using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Noise;
using GlimmerWorks.Engine.World.Terrain;

namespace GlimmerWorks.Engine.World.Generation;

/// <summary>
/// <para>IBiomeDecorator</para>
/// </summary>
public interface IBiomeDecorator
{
    void Apply(ChunkGenerationContext context, DecoratorResource decorator, BiomeResource biome, NoiseSampler noise);
}

/// <summary>
/// <para>DecoratorRegistry</para>
/// </summary>
public sealed class DecoratorRegistry
{
    private readonly Dictionary<string, IBiomeDecorator> _decorators =
        new(StringComparer.OrdinalIgnoreCase)
        {
            ["fill"] = new FillDecorator(),
            ["ore"] = new OreDecorator(),
            ["replace"] = new ReplaceDecorator(),
            ["loot_chest"] = new LootChestDecorator()
        };

    public IBiomeDecorator? Find(string type) => type.ToLowerInvariant() switch
    {
        "0" => _decorators.GetValueOrDefault("fill"),
        "1" => _decorators.GetValueOrDefault("ore"),
        "2" => _decorators.GetValueOrDefault("replace"),
        "3" => _decorators.GetValueOrDefault("loot_chest"),
        _ => _decorators.GetValueOrDefault(type)
    };

    public void Register(string type, IBiomeDecorator decorator) => _decorators[type] = decorator;
}

/// <summary>
/// <para>FillDecorator</para>
/// </summary>
public sealed class FillDecorator : IBiomeDecorator
{
    public void Apply(ChunkGenerationContext context, DecoratorResource decorator, BiomeResource biome,
        NoiseSampler noise)
    {
        var layerIndex = decorator.LayerIndex;
        if (layerIndex < 0)
        {
            return;
        }

        var target = ChunkGenerationContext.Normalize(decorator.GetEffectiveTile());
        for (var x = 0; x < WorldConstants.ChunkSize; x++)
        {
            for (var y = 0; y < WorldConstants.ChunkSize; y++)
            {
                var index = WorldConstants.ChunkIndex(x, y);
                var cell = context.GetTerrainCell(x, y);
                if (cell.Type != TerrainType.Solid || !MatchBiome(cell.Biome, biome))
                {
                    continue;
                }

                context.References.SetByIndex(layerIndex, index, target);
            }
        }
    }

    internal static bool MatchBiome(BiomeResource? left, BiomeResource right) =>
        left is not null && left.PackId == right.PackId && left.ResourceId == right.ResourceId;
}

/// <summary>
/// <para>OreDecorator</para>
/// </summary>
public sealed class OreDecorator : IBiomeDecorator
{
    public void Apply(ChunkGenerationContext context, DecoratorResource decorator, BiomeResource biome,
        NoiseSampler noise)
    {
        var layerIndex = decorator.LayerIndex;
        if (layerIndex < 0)
        {
            return;
        }

        var target = ChunkGenerationContext.Normalize(decorator.GetEffectiveTile());
        if (target is null)
        {
            return;
        }

        var dimension = context.Dimension;
        var span = dimension.MaxY - dimension.MinY;
        var heightRange = decorator.MaxHeight - decorator.MinHeight;
        if (span <= 0 || heightRange <= 0)
        {
            return;
        }

        var count = 0;
        var maxCount = decorator.MaxCount <= 0 ? 256 : decorator.MaxCount;
        for (var localX = 0; localX < WorldConstants.ChunkSize; localX++)
        {
            for (var localY = 0; localY < WorldConstants.ChunkSize; localY++)
            {
                var index = WorldConstants.ChunkIndex(localX, localY);
                var cell = context.GetTerrainCell(localX, localY);
                var worldCell = context.GetWorldCell(localX, localY);

                var elevation = (worldCell.Y - dimension.MinY) / (float)span;
                if (elevation < decorator.MinHeight || elevation > decorator.MaxHeight)
                {
                    continue;
                }

                if (cell.Type != TerrainType.Solid || !FillDecorator.MatchBiome(cell.Biome, biome))
                {
                    continue;
                }

                var value = noise.SampleNormalized(worldCell.X, worldCell.Y);
                var elevationRatio = (elevation - decorator.MinHeight) / heightRange;
                var threshold = decorator.ChanceIncreasesWithDepth
                    ? Lerp(decorator.MinNoiseThreshold, decorator.MaxNoiseThreshold, elevationRatio)
                    : Lerp(decorator.MaxNoiseThreshold, decorator.MinNoiseThreshold, elevationRatio);

                if (value <= threshold)
                {
                    continue;
                }

                context.References.SetByIndex(layerIndex, index, target);
                count++;
                if (count >= maxCount)
                {
                    return;
                }
            }
        }
    }

    private static float Lerp(float from, float to, float t) => from + (to - from) * Math.Clamp(t, 0.0F, 1.0F);
}

/// <summary>
/// <para>ReplaceDecorator</para>
/// </summary>
public sealed class ReplaceDecorator : IBiomeDecorator
{
    public void Apply(ChunkGenerationContext context, DecoratorResource decorator, BiomeResource biome,
        NoiseSampler noise)
    {
        var layerIndex = decorator.LayerIndex;
        if (layerIndex < 0)
        {
            return;
        }

        for (var localX = 0; localX < WorldConstants.ChunkSize; localX++)
        {
            for (var localY = 0; localY < WorldConstants.ChunkSize; localY++)
            {
                var index = WorldConstants.ChunkIndex(localX, localY);
                var cell = context.GetTerrainCell(localX, localY);
                if (cell.Type != TerrainType.Solid || !FillDecorator.MatchBiome(cell.Biome, biome))
                {
                    continue;
                }

                var currentCell = context.GetWorldCell(localX, localY);
            foreach (var rule in decorator.Rules)
            {
                var target = ChunkGenerationContext.Normalize(rule.TargetTile);
                if (target is null)
                {
                    continue;
                }

                var sourceTile = ChunkGenerationContext.Normalize(rule.SourceTile);
                if (sourceTile is null && rule.SourceTerrainMask == 0)
                {
                    // Empty rule.
                    continue;
                }

                var detectionCell = new TileVector2D(currentCell.X + rule.OffsetX, currentCell.Y + rule.OffsetY);

                if (rule.SourceTerrainMask != 0)
                {
                    if (!context.TryGetTerrainCell(detectionCell, out var detectionTerrain))
                    {
                        continue;
                    }

                    if ((rule.SourceTerrainMask & (int)detectionTerrain.Type) == 0)
                    {
                        continue;
                    }
                }

                if (sourceTile is not null)
                {
                    // Source tiles can only be checked inside this chunk.
                    var detectionLocalX = detectionCell.X - context.ChunkPosition.X;
                    var detectionLocalY = detectionCell.Y - context.ChunkPosition.Y;
                    if (detectionLocalX is < 0 or >= WorldConstants.ChunkSize ||
                        detectionLocalY is < 0 or >= WorldConstants.ChunkSize)
                    {
                        continue;
                    }

                    var detectionIndex = WorldConstants.ChunkIndex(detectionLocalX, detectionLocalY);
                    var actual = context.References.GetByIndex(layerIndex, detectionIndex);
                    if (!ChunkGenerationContext.SameReference(actual, sourceTile))
                    {
                        continue;
                    }
                }

                context.References.SetByIndex(layerIndex, index, target);
                break;
                }
            }
        }
    }
}

/// <summary>
/// <para>LootChestDecorator</para>
/// </summary>
public sealed class LootChestDecorator : IBiomeDecorator
{
    public void Apply(ChunkGenerationContext context, DecoratorResource decorator, BiomeResource biome,
        NoiseSampler noise)
    {
        var layerIndex = decorator.LayerIndex;
        if (layerIndex < 0)
        {
            return;
        }

        var target = ChunkGenerationContext.Normalize(decorator.GetEffectiveTile());
        if (target is null)
        {
            return;
        }

        var dimension = context.Dimension;
        var span = dimension.MaxY - dimension.MinY;
        if (span <= 0)
        {
            return;
        }

        // The chest schema defaults to full height; the shared resource default is ore-oriented.
        var maxHeight = decorator.MaxHeight <= 0.5F && decorator.MinHeight <= 0.0F
            ? 1.0F
            : decorator.MaxHeight;

        var count = 0;
        for (var y = 0; y < WorldConstants.ChunkSize; y++)
        {
            for (var x = 0; x < WorldConstants.ChunkSize; x++)
            {
                var index = WorldConstants.ChunkIndex(x, y);
                var cell = context.GetTerrainCell(x, y);
                if (!FillDecorator.MatchBiome(cell.Biome, biome))
                {
                    continue;
                }

                var worldCell = context.GetWorldCell(x, y);
                var elevation = (worldCell.Y - dimension.MinY) / (float)span;
                if (elevation < decorator.MinHeight || elevation > maxHeight)
                {
                    continue;
                }

                if (decorator.RequireSurface)
                {
                    if (y + 1 >= WorldConstants.ChunkSize)
                    {
                        continue;
                    }

                    var aboveIndex = WorldConstants.ChunkIndex(x, y + 1);
                    if (!context.IsAirReference(context.References.GetByIndex(layerIndex, aboveIndex)))
                    {
                        continue;
                    }

                    if (context.IsAirReference(context.References.GetByIndex(layerIndex, index)))
                    {
                        continue;
                    }
                }

                var value = noise.SampleNormalized(worldCell.X, worldCell.Y);
                if (value <= decorator.GenerationThreshold)
                {
                    continue;
                }

                context.References.SetByIndex(layerIndex, index, target);
                count++;
                if (decorator.MaxCount > 0 && count >= decorator.MaxCount)
                {
                    return;
                }
            }
        }
    }
}
