using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>TerrainGenerator</para>
/// </summary>
/// <remarks>
/// <para>Terrain blocks are cached weakly. A block stays alive while any chunk inside it holds a strong reference.</para>
/// </remarks>
public sealed class TerrainGenerator
{
    private readonly DimensionResource _dimension;
    private readonly ResourceRef _dimensionRef;
    private readonly long _worldSeed;
    private readonly DataRepository _dataRepository;
    private readonly IReadOnlyList<BiomeResource> _biomes;
    private readonly IReadOnlyList<StructureResource> _structures;
    private readonly StructureGeneratorRegistry _generatorRegistry;
    private readonly StructureConditionRegistry _conditionRegistry;
    private readonly Dictionary<TerrainVertexVector2D, WeakReference<TerrainBlock>> _cache = [];
    private readonly object _cacheLock = new();

    public TerrainGenerator(
        DimensionResource dimension,
        ResourceRef dimensionRef,
        long worldSeed,
        DataRepository dataRepository,
        DimensionNoiseSet noise,
        IEnumerable<BiomeResource> biomes,
        IEnumerable<StructureResource> structures,
        StructureGeneratorRegistry? generatorRegistry = null,
        StructureConditionRegistry? conditionRegistry = null)
    {
        _dimension = dimension;
        _dimensionRef = dimensionRef;
        _worldSeed = worldSeed;
        _dataRepository = dataRepository;
        _biomes = biomes.ToList();
        _structures = structures.ToList();
        _generatorRegistry = generatorRegistry ?? new StructureGeneratorRegistry();
        _conditionRegistry = conditionRegistry ?? new StructureConditionRegistry();
        Climate = new ClimateSampler(dimension, noise);
        Matcher = new BiomeMatcher(_biomes, dimensionRef);
    }

    public ClimateSampler Climate { get; }

    public BiomeMatcher Matcher { get; }

    public DimensionResource Dimension => _dimension;

    public long WorldSeed => _worldSeed;

    /// <summary>
    /// <para>Get a cached terrain block or generate it.</para>
    /// </summary>
    public TerrainBlock GetOrGenerate(TerrainVertexVector2D position)
    {
        lock (_cacheLock)
        {
            if (_cache.TryGetValue(position, out var weak) && weak.TryGetTarget(out var cached))
            {
                return cached;
            }

            var generated = Generate(position);
            _cache[position] = new WeakReference<TerrainBlock>(generated);
            return generated;
        }
    }

    /// <summary>
    /// <para>Remove dead cache entries.</para>
    /// </summary>
    public void CollectCache()
    {
        lock (_cacheLock)
        {
            var dead = _cache.Where(static pair => !pair.Value.TryGetTarget(out _))
                .Select(static pair => pair.Key)
                .ToList();
            foreach (var key in dead)
            {
                _cache.Remove(key);
            }
        }
    }

    /// <summary>
    /// <para>Generate a terrain block from scratch.</para>
    /// </summary>
    public TerrainBlock Generate(TerrainVertexVector2D position)
    {
        var block = new TerrainBlock(position);
        var surfaceHeights = new int[WorldConstants.TerrainSize];
        for (var x = 0; x < WorldConstants.TerrainSize; x++)
        {
            surfaceHeights[x] = Climate.GetSurfaceHeight(position.X + x);
        }

        for (var localY = 0; localY < WorldConstants.TerrainSize; localY++)
        {
            var worldY = position.Y + localY;
            for (var localX = 0; localX < WorldConstants.TerrainSize; localX++)
            {
                var worldX = position.X + localX;
                var index = WorldConstants.TerrainIndex(localX, localY);
                var cell = new TerrainCell { Position = new TileVector2D(worldX, worldY) };

                if (!WorldConstants.InDimension(_dimension, worldX, worldY))
                {
                    cell.Type = TerrainType.Empty;
                    block.Cells[index] = cell;
                    continue;
                }

                var surfaceHeight = surfaceHeights[localX];
                var climate = Climate.Sample(worldX, worldY, surfaceHeight);
                cell.Biome = Matcher.Find(climate);

                if (worldY == _dimension.MinY)
                {
                    cell.Type = TerrainType.Bedrock;
                    block.Cells[index] = cell;
                    continue;
                }

                if (worldY > surfaceHeight)
                {
                    cell.Type = worldY < _dimension.SeaLevelY ? TerrainType.Water : TerrainType.Air;
                    block.Cells[index] = cell;
                    continue;
                }

                cell.Type = TerrainType.Solid;
                block.Cells[index] = cell;
            }
        }

        PlaceStructures(block);
        return block;
    }

    private void PlaceStructures(TerrainBlock block)
    {
        if (_structures.Count == 0)
        {
            // Unlike the historical implementation, an empty structure registry does not fail terrain generation.
            return;
        }

        var occupied = new bool[WorldConstants.TerrainCellCount];
        Array.Fill(occupied, true);
        var conditionMask = new bool[WorldConstants.TerrainCellCount];
        var structureMask = new bool[WorldConstants.TerrainCellCount];
        var context = new StructureGenerationContext(block, _dimension, _dimensionRef, Climate, _worldSeed);

        foreach (var structure in _structures
                     .Select((value, index) => (value, index))
                     .OrderByDescending(static pair => pair.value.Priority)
                     .ThenBy(static pair => pair.index)
                     .Select(static pair => pair.value))
        {
            if (structure.Conditions.Count == 0)
            {
                continue;
            }

            var generator = _generatorRegistry.Find(structure.StructureType);
            if (generator is null)
            {
                LogCat.Warn($"Unknown structure generator type: {structure.StructureType}.");
                continue;
            }

            Array.Fill(structureMask, true);
            var valid = true;
            foreach (var conditionRef in structure.Conditions)
            {
                var condition = _dataRepository.Get<StructureConditionResource>(conditionRef);
                if (condition is null)
                {
                    valid = false;
                    break;
                }

                var processor = _conditionRegistry.Find(condition.Processor);
                if (processor is null)
                {
                    valid = false;
                    break;
                }

                Array.Fill(conditionMask, false);
                processor.Fill(context, condition, conditionMask);
                for (var index = 0; index < WorldConstants.TerrainCellCount; index++)
                {
                    structureMask[index] &= conditionMask[index];
                }
            }

            if (!valid)
            {
                continue;
            }

            for (var index = 0; index < WorldConstants.TerrainCellCount; index++)
            {
                if (!structureMask[index] || !occupied[index])
                {
                    continue;
                }

                var origin = new TileVector2D(
                    block.Position.X + WorldConstants.TerrainIndexX(index),
                    block.Position.Y + WorldConstants.TerrainIndexY(index));
                TryPlaceStructure(block, context, structure, generator, origin, occupied);
            }
        }
    }

    private static void TryPlaceStructure(
        TerrainBlock block,
        StructureGenerationContext context,
        StructureResource structure,
        ITerrainStructureGenerator generator,
        TileVector2D origin,
        bool[] occupied)
    {
        var info = generator.Generate(context, origin, structure);
        if (info is null || info.Placements.Count == 0)
        {
            return;
        }

        var staging = block.StructureStaging;
        staging.Clear();
        var conflict = false;

        foreach (var placement in info.Placements)
        {
            if (WorldConstants.LayerToIndex(placement.Layer) < 0 ||
                string.IsNullOrEmpty(placement.Tile.ResourceId))
            {
                staging.Clear();
                return;
            }

            var absolute = new TileVector2D(origin.X + placement.X, origin.Y + placement.Y);
            var index = block.IndexOf(absolute);
            if (index < 0)
            {
                // Structures must fit completely inside the current terrain block.
                staging.Clear();
                return;
            }

            if (!occupied[index])
            {
                conflict = true;
                break;
            }

            staging.Add(placement);
        }

        if (conflict)
        {
            // Structures are committed as a whole or discarded as a whole.
            staging.Clear();
            return;
        }

        foreach (var placement in staging)
        {
            var absolute = new TileVector2D(origin.X + placement.X, origin.Y + placement.Y);
            var index = block.IndexOf(absolute);
            if (index < 0)
            {
                continue;
            }

            var cell = block.Cells[index];
            cell.Type = TerrainType.Structure;
            cell.SetStructureTile(WorldConstants.LayerToIndex(placement.Layer), placement.Tile);
            block.Cells[index] = cell;
            occupied[index] = false;
        }

        staging.Clear();
    }
}
