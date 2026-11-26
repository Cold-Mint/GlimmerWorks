using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Noise;
using GlimmerWorks.Engine.World.Terrain;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Generation;

/// <summary>
/// <para>ChunkGenerator</para>
/// </summary>
/// <remarks>
/// <para>The three steps are fixed and must not be reordered.</para>
/// </remarks>
public sealed class ChunkGenerator
{
    private readonly ITileRegistry _tiles;
    private readonly DimensionResource _dimension;
    private readonly ResourceRef _dimensionRef;
    private readonly long _worldSeed;
    private readonly ClimateSampler _climate;
    private readonly DataRepository _dataRepository;
    private readonly DecoratorRegistry _decorators;
    private readonly TileInstancePool _pool;
    private readonly Dictionary<string, NoiseSampler> _decoratorNoise = [];

    public ChunkGenerator(
        ITileRegistry tiles,
        DimensionResource dimension,
        ResourceRef dimensionRef,
        long worldSeed,
        ClimateSampler climate,
        DataRepository dataRepository,
        TileInstancePool pool,
        DecoratorRegistry? decorators = null)
    {
        _tiles = tiles;
        _dimension = dimension;
        _dimensionRef = dimensionRef;
        _worldSeed = worldSeed;
        _climate = climate;
        _dataRepository = dataRepository;
        _pool = pool;
        _decorators = decorators ?? new DecoratorRegistry();
    }

    public DecoratorRegistry Decorators => _decorators;

    /// <summary>
    /// <para>Generate a chunk from a terrain block.</para>
    /// </summary>
    public Chunk Generate(ChunkVertexVector2D position, TerrainBlock terrain)
    {
        var context = new ChunkGenerationContext(
            position, terrain, _dimension, _dimensionRef, _worldSeed, _climate, _tiles);

        ResolveTerrainReferences(context);
        ApplyDecorators(context);
        return FillTiles(context);
    }

    private void ResolveTerrainReferences(ChunkGenerationContext context)
    {
        var dimension = _dimension;
        var voidRefs = new[]
        {
            ChunkGenerationContext.Normalize(dimension.VoidWall),
            ChunkGenerationContext.Normalize(dimension.VoidDecoration),
            ChunkGenerationContext.Normalize(dimension.VoidTile)
        };
        var airRefs = new[]
        {
            ChunkGenerationContext.Normalize(dimension.AirWall),
            ChunkGenerationContext.Normalize(dimension.AirDecoration),
            ChunkGenerationContext.Normalize(dimension.AirTile)
        };
        var waterRef = ChunkGenerationContext.Normalize(dimension.WaterTile);
        var bedrockRef = ChunkGenerationContext.Normalize(dimension.BedrockTile);

        for (var y = 0; y < WorldConstants.ChunkSize; y++)
        {
            for (var x = 0; x < WorldConstants.ChunkSize; x++)
            {
                var index = WorldConstants.ChunkIndex(x, y);
                var cell = context.GetTerrainCell(x, y);

                switch (cell.Type)
                {
                    case TerrainType.Empty:
                        SetAll(context, index, voidRefs);
                        break;

                    case TerrainType.Air:
                        SetAll(context, index, airRefs);
                        break;

                    case TerrainType.Water:
                        context.References.SetByIndex(0, index, airRefs[0]);
                        context.References.SetByIndex(1, index, airRefs[1]);
                        context.References.SetByIndex(2, index, waterRef);
                        break;

                    case TerrainType.Solid:
                        SetAll(context, index, airRefs);
                        context.AddBiome(cell.Biome);
                        break;

                    case TerrainType.Structure:
                        context.References.SetByIndex(0, index,
                            ChunkGenerationContext.Normalize(cell.GetStructureTile(0)) ?? airRefs[0]);
                        context.References.SetByIndex(1, index,
                            ChunkGenerationContext.Normalize(cell.GetStructureTile(1)) ?? airRefs[1]);
                        context.References.SetByIndex(2, index,
                            ChunkGenerationContext.Normalize(cell.GetStructureTile(2)) ?? airRefs[2]);
                        break;

                    case TerrainType.Bedrock:
                        context.References.SetByIndex(0, index, voidRefs[0]);
                        context.References.SetByIndex(1, index, voidRefs[1]);
                        context.References.SetByIndex(2, index, bedrockRef ?? voidRefs[2]);
                        break;

                    default:
                        SetAll(context, index, airRefs);
                        break;
                }
            }
        }
    }

    private static void SetAll(ChunkGenerationContext context, int index, ResourceRef?[] references)
    {
        context.References.SetByIndex(0, index, references[0]);
        context.References.SetByIndex(1, index, references[1]);
        context.References.SetByIndex(2, index, references[2]);
    }

    private void ApplyDecorators(ChunkGenerationContext context)
    {
        foreach (var biome in context.ChunkBiomes)
        {
            foreach (var decoratorRef in biome.Decorators)
            {
                var decorator = _dataRepository.Get<DecoratorResource>(decoratorRef);
                if (decorator is null)
                {
                    continue;
                }

                var implementation = _decorators.Find(decorator.DecoratorType);
                if (implementation is null)
                {
                    LogCat.Warn($"Unknown decorator type: {decorator.DecoratorType}.");
                    continue;
                }

                implementation.Apply(context, decorator, biome, GetDecoratorNoise(decorator));
            }
        }
    }

    private Chunk FillTiles(ChunkGenerationContext context)
    {
        var chunk = new Chunk(context.ChunkPosition, _tiles, _pool) { Terrain = context.Terrain };
        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            var layer = WorldConstants.IndexToLayer(layerIndex);
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var reference = context.References.GetByIndex(layerIndex, index);
                if (reference is null)
                {
                    continue;
                }

                var resource = _tiles.Find(reference);
                if (resource is null)
                {
                    continue;
                }

                if (resource.LayerIndex != layerIndex)
                {
                    LogCat.Warn(
                        $"Tile layer mismatch (resource={resource.ResourceId}, declared={resource.Layer}, used={layer}).");
                    continue;
                }

                var height = resource.ClampedHeight;
                var topLeftIndex = index + (height - 1) * WorldConstants.ChunkSize;
                chunk.Place(layer, topLeftIndex, reference, resource,
                    TileDestroySource.WorldGeneration, TilePlaceSource.WorldGeneration);
            }
        }

        return chunk;
    }

    private NoiseSampler GetDecoratorNoise(DecoratorResource decorator)
    {
        var key = $"{decorator.PackId}:{decorator.ResourceId}";
        if (_decoratorNoise.TryGetValue(key, out var cached))
        {
            return cached;
        }

        var type = decorator.DecoratorType.ToLowerInvariant();
        var defaultOffset = type is "ore" or "1" ? 1024 : type is "loot_chest" or "3" ? 4096 : 0;
        var offset = decorator.SeedOffset == 0 ? defaultOffset : decorator.SeedOffset;
        var defaultFrequency = type is "loot_chest" or "3" ? 0.02F : 0.01F;
        var frequency = decorator.NoiseFrequency > 0 ? decorator.NoiseFrequency : defaultFrequency;

        NoiseSampler sampler;
        if (!string.IsNullOrEmpty(decorator.Noise.ResourceId))
        {
            var noiseResource = _dataRepository.Get<NoiseResource>(decorator.Noise);
            sampler = noiseResource is null
                ? NoiseSampler.CreateDefault(_worldSeed, offset, frequency, 3)
                : NoiseSampler.Create(noiseResource, _worldSeed + offset);
        }
        else
        {
            sampler = NoiseSampler.CreateDefault(_worldSeed, offset, frequency, 3);
        }

        _decoratorNoise[key] = sampler;
        return sampler;
    }
}
