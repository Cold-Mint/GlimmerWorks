using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>StructurePlacement</para>
/// </summary>
public readonly record struct StructurePlacement(ResourceRef Tile, int X, int Y, int Layer);

/// <summary>
/// <para>StructureInfo</para>
/// </summary>
public sealed class StructureInfo
{
    public List<StructurePlacement> Placements { get; } = [];
}

/// <summary>
/// <para>StructureGenerationContext</para>
/// </summary>
public sealed class StructureGenerationContext(
    TerrainBlock terrain,
    DimensionResource dimension,
    ResourceRef dimensionRef,
    ClimateSampler climate,
    long worldSeed)
{
    public TerrainBlock Terrain { get; } = terrain;

    public DimensionResource Dimension { get; } = dimension;

    public ResourceRef DimensionRef { get; } = dimensionRef;

    public ClimateSampler Climate { get; } = climate;

    public long WorldSeed { get; } = worldSeed;

    /// <summary>
    /// <para>Humidity at a world cell.</para>
    /// </summary>
    public float GetHumidity(int worldX, int worldY) =>
        Climate.Sample(worldX, worldY, Climate.GetSurfaceHeight(worldX)).Humidity;
}

/// <summary>
/// <para>ITerrainStructureGenerator</para>
/// </summary>
public interface ITerrainStructureGenerator
{
    StructureInfo? Generate(StructureGenerationContext context, TileVector2D origin,
        StructureResource structure);
}

/// <summary>
/// <para>ITerrainStructureConditionProcessor</para>
/// </summary>
public interface ITerrainStructureConditionProcessor
{
    /// <summary>
    /// <para>Fill a mask where true means the cell is selected by this condition.</para>
    /// </summary>
    void Fill(StructureGenerationContext context, StructureConditionResource condition, bool[] mask);
}

/// <summary>
/// <para>StaticStructureGenerator</para>
/// </summary>
public sealed class StaticStructureGenerator : ITerrainStructureGenerator
{
    public StructureInfo? Generate(StructureGenerationContext context, TileVector2D origin,
        StructureResource structure)
    {
        var info = new StructureInfo();
        foreach (var tile in structure.Tiles)
        {
            info.Placements.Add(new StructurePlacement(tile.Tile, tile.X, tile.Y, tile.Layer));
        }

        return info;
    }
}

/// <summary>
/// <para>TreeStructureGenerator</para>
/// </summary>
public sealed class TreeStructureGenerator : ITerrainStructureGenerator
{
    public StructureInfo? Generate(StructureGenerationContext context, TileVector2D origin,
        StructureResource structure)
    {
        var info = new StructureInfo();
        var minHeight = Math.Max(1, structure.MinTrunkHeight);
        var maxHeight = Math.Max(minHeight, structure.MaxTrunkHeight);
        var humidity = context.GetHumidity(origin.X, origin.Y);
        var trunkHeight = minHeight + (int)(humidity * (maxHeight - minHeight));
        var trunkWidth = Math.Clamp(structure.TrunkWidth, 1, 16);
        var trunkTile = structure.GetTrunkTile();

        for (var y = 1; y <= trunkHeight; y++)
        {
            for (var x = 0; x < trunkWidth; x++)
            {
                info.Placements.Add(new StructurePlacement(trunkTile, x, y, structure.TrunkLayer));
            }
        }

        if (!structure.HasLeaves || structure.LeafClusters <= 0)
        {
            return info;
        }

        var radius = Math.Max(0, structure.LeafRadius);
        var leavesTile = structure.GetLeavesTile();
        for (var cluster = 0; cluster < structure.LeafClusters; cluster++)
        {
            var centerY = trunkHeight + 1 - cluster * Math.Max(1, structure.LeafVerticalSpacing);
            for (var x = -radius; x <= radius; x++)
            {
                for (var y = -radius; y <= radius; y++)
                {
                    if (x * x + y * y > radius * radius || y < 0)
                    {
                        continue;
                    }

                    info.Placements.Add(new StructurePlacement(
                        leavesTile,
                        x + trunkWidth / 2,
                        centerY + y,
                        structure.LeavesLayer));
                }
            }
        }

        return info;
    }
}

/// <summary>
/// <para>StructureGeneratorRegistry</para>
/// </summary>
public sealed class StructureGeneratorRegistry
{
    private readonly Dictionary<string, ITerrainStructureGenerator> _generators =
        new(StringComparer.OrdinalIgnoreCase)
        {
            ["static"] = new StaticStructureGenerator(),
            ["tree"] = new TreeStructureGenerator()
        };

    public ITerrainStructureGenerator? Find(string type) => type.ToLowerInvariant() switch
    {
        "1" => _generators.GetValueOrDefault("static"),
        "2" => _generators.GetValueOrDefault("tree"),
        "0" or "none" => null,
        _ => _generators.GetValueOrDefault(type)
    };

    public void Register(string type, ITerrainStructureGenerator generator) =>
        _generators[type] = generator;
}

/// <summary>
/// <para>StructureConditionRegistry</para>
/// </summary>
public sealed class StructureConditionRegistry
{
    private readonly Dictionary<string, ITerrainStructureConditionProcessor> _processors =
        new(StringComparer.OrdinalIgnoreCase)
        {
            ["biome"] = new BiomeConditionProcessor(),
            ["height"] = new HeightConditionProcessor(),
            ["spacing"] = new SpacingConditionProcessor(),
            ["surface"] = new SurfaceConditionProcessor()
        };

    public ITerrainStructureConditionProcessor? Find(string type) => type.ToLowerInvariant() switch
    {
        "1" => _processors.GetValueOrDefault("biome"),
        "2" => _processors.GetValueOrDefault("height"),
        "3" => _processors.GetValueOrDefault("spacing"),
        "4" => _processors.GetValueOrDefault("surface"),
        "0" or "none" => null,
        _ => _processors.GetValueOrDefault(type)
    };

    public void Register(string type, ITerrainStructureConditionProcessor processor) =>
        _processors[type] = processor;
}

/// <summary>
/// <para>BiomeConditionProcessor</para>
/// </summary>
public sealed class BiomeConditionProcessor : ITerrainStructureConditionProcessor
{
    public void Fill(StructureGenerationContext context, StructureConditionResource condition, bool[] mask)
    {
        var terrain = context.Terrain;
        var accepted = condition.Biomes
            .Select(static biome => (biome.TargetPackId, biome.ResourceId))
            .ToHashSet();

        for (var index = 0; index < WorldConstants.TerrainCellCount; index++)
        {
            if (mask[index])
            {
                continue;
            }

            var cell = terrain.Cells[index];
            if (cell.Type != TerrainType.Solid || cell.Biome is null)
            {
                continue;
            }

            if (accepted.Contains((cell.Biome.PackId, cell.Biome.ResourceId)))
            {
                mask[index] = true;
            }
        }
    }
}

/// <summary>
/// <para>HeightConditionProcessor</para>
/// </summary>
public sealed class HeightConditionProcessor : ITerrainStructureConditionProcessor
{
    public void Fill(StructureGenerationContext context, StructureConditionResource condition, bool[] mask)
    {
        var maxY = context.Dimension.MaxY;
        if (maxY <= 0)
        {
            return;
        }

        for (var index = 0; index < WorldConstants.TerrainCellCount; index++)
        {
            if (mask[index])
            {
                continue;
            }

            var worldY = context.Terrain.Position.Y + WorldConstants.TerrainIndexY(index);
            var percentage = worldY / (float)maxY;
            if (percentage >= condition.MinPercent && percentage <= condition.MaxPercent)
            {
                mask[index] = true;
            }
        }
    }
}

/// <summary>
/// <para>SpacingConditionProcessor</para>
/// </summary>
public sealed class SpacingConditionProcessor : ITerrainStructureConditionProcessor
{
    public void Fill(StructureGenerationContext context, StructureConditionResource condition, bool[] mask)
    {
        var spacing = condition.MinSpacing <= 0 ? 4 : condition.MinSpacing;
        var axis = condition.Vertical ? "y" : condition.Axis.ToLowerInvariant();

        for (var index = 0; index < WorldConstants.TerrainCellCount; index++)
        {
            if (mask[index])
            {
                continue;
            }

            var worldX = context.Terrain.Position.X + WorldConstants.TerrainIndexX(index);
            var worldY = context.Terrain.Position.Y + WorldConstants.TerrainIndexY(index);
            var matches = axis switch
            {
                "y" => worldY % spacing == 0,
                "both" => worldX % spacing == 0 && worldY % spacing == 0,
                _ => worldX % spacing == 0
            };

            if (matches)
            {
                mask[index] = true;
            }
        }
    }
}

/// <summary>
/// <para>SurfaceConditionProcessor</para>
/// </summary>
public sealed class SurfaceConditionProcessor : ITerrainStructureConditionProcessor
{
    public void Fill(StructureGenerationContext context, StructureConditionResource condition, bool[] mask)
    {
        var terrain = context.Terrain;
        for (var y = 0; y < WorldConstants.TerrainSize - 1; y++)
        {
            for (var x = 0; x < WorldConstants.TerrainSize; x++)
            {
                var index = WorldConstants.TerrainIndex(x, y);
                if (mask[index])
                {
                    continue;
                }

                var cell = terrain.Cells[index];
                if (cell.Type != TerrainType.Solid)
                {
                    continue;
                }

                if (terrain.Cells[WorldConstants.TerrainIndex(x, y + 1)].Type == TerrainType.Air)
                {
                    mask[index] = true;
                }
            }
        }
    }
}
