using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Terrain;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Generation;

/// <summary>
/// <para>ChunkGenerationContext</para>
/// </summary>
public sealed class ChunkGenerationContext(
    ChunkVertexVector2D chunkPosition,
    TerrainBlock terrain,
    DimensionResource dimension,
    ResourceRef dimensionRef,
    long worldSeed,
    ClimateSampler climate,
    ITileRegistry tiles)
{
    public ChunkVertexVector2D ChunkPosition { get; } = chunkPosition;

    public TerrainBlock Terrain { get; } = terrain;

    public DimensionResource Dimension { get; } = dimension;

    public ResourceRef DimensionRef { get; } = dimensionRef;

    public long WorldSeed { get; } = worldSeed;

    public ClimateSampler Climate { get; } = climate;

    public ITileRegistry Tiles { get; } = tiles;

    public TileReferenceTable References { get; } = new();

    /// <summary>
    /// <para>Biomes that occur in this chunk, in deterministic first-occurrence order.</para>
    /// </summary>
    public List<BiomeResource> ChunkBiomes { get; } = [];

    /// <summary>
    /// <para>World cell of a chunk-local coordinate.</para>
    /// </summary>
    public TileVector2D GetWorldCell(int localX, int localY) => new(
        ChunkPosition.X + localX,
        ChunkPosition.Y + localY);

    /// <summary>
    /// <para>Chunk index of a chunk-local coordinate.</para>
    /// </summary>
    public int GetChunkIndex(int localX, int localY) => WorldConstants.ChunkIndex(localX, localY);

    /// <summary>
    /// <para>Get the terrain cell under a chunk-local coordinate.</para>
    /// </summary>
    public TerrainCell GetTerrainCell(int localX, int localY) =>
        Terrain.GetCell(new TileVector2D(ChunkPosition.X + localX, ChunkPosition.Y + localY)) ?? default;

    /// <summary>
    /// <para>Try to query the terrain result at a world cell. Only the owning terrain block is queryable.</para>
    /// </summary>
    public bool TryGetTerrainCell(TileVector2D worldCell, out TerrainCell cell) =>
        Terrain.TryGetCell(worldCell, out cell);

    /// <summary>
    /// <para>Whether a reference represents an air cell.</para>
    /// </summary>
    public bool IsAirReference(ResourceRef? reference)
    {
        if (reference is null || string.IsNullOrEmpty(reference.ResourceId))
        {
            return true;
        }

        return SameReference(reference, Dimension.AirTile);
    }

    /// <summary>
    /// <para>Whether two references point at the same resource.</para>
    /// </summary>
    public static bool SameReference(ResourceRef? left, ResourceRef? right)
    {
        if (left is null || right is null)
        {
            return left is null && right is null;
        }

        return left.TargetPackId == right.TargetPackId &&
               left.ResourceId == right.ResourceId &&
               left.ResourceType == right.ResourceType;
    }

    /// <summary>
    /// <para>Normalize an empty reference to null.</para>
    /// </summary>
    public static ResourceRef? Normalize(ResourceRef? reference) =>
        reference is null || string.IsNullOrEmpty(reference.ResourceId) ? null : reference;

    /// <summary>
    /// <para>Add a biome to the chunk set if absent, preserving order.</para>
    /// </summary>
    public void AddBiome(BiomeResource? biome)
    {
        if (biome is null)
        {
            return;
        }

        if (ChunkBiomes.Any(candidate => candidate.PackId == biome.PackId &&
                                        candidate.ResourceId == biome.ResourceId))
        {
            return;
        }

        ChunkBiomes.Add(biome);
    }
}
