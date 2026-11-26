using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.World.Generation;
using GlimmerWorks.Engine.World.Lighting;
using GlimmerWorks.Engine.World.Terrain;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Storage;

/// <summary>
/// <para>ChunkStore</para>
/// </summary>
/// <remarks>
/// <para>The chunk table is protected by a lock. Chunk-internal arrays, tile states and the light buffer have their own ownership rules.</para>
/// </remarks>
public sealed class ChunkStore
{
    private readonly Dictionary<ChunkVertexVector2D, Chunk> _chunks = [];
    private readonly object _tableLock = new();
    private readonly ITileRegistry _tiles;
    private readonly TileInstancePool _pool;
    private readonly TerrainGenerator _terrainGenerator;
    private readonly ChunkGenerator _chunkGenerator;
    private readonly IChunkStorage _storage;
    private readonly string _dimensionId;

    public ChunkStore(
        string dimensionId,
        ITileRegistry tiles,
        TileInstancePool pool,
        TerrainGenerator terrainGenerator,
        ChunkGenerator chunkGenerator,
        IChunkStorage storage)
    {
        _dimensionId = dimensionId;
        _tiles = tiles;
        _pool = pool;
        _terrainGenerator = terrainGenerator;
        _chunkGenerator = chunkGenerator;
        _storage = storage;
    }

    /// <summary>
    /// <para>Owning world, assigned after construction.</para>
    /// </summary>
    public GlimmerWorks.Engine.World.World? World { get; set; }

    /// <summary>
    /// <para>Light buffer driven by tile changes.</para>
    /// </summary>
    public LightBuffer? Light { get; set; }

    /// <summary>
    /// <para>Attachment cascade handler.</para>
    /// </summary>
    public Attachments.AttachmentSystem? Attachments { get; set; }

    /// <summary>
    /// <para>Growth system used to catch up offline growth for loaded saves.</para>
    /// </summary>
    public Systems.GrowthSystem? Growth { get; set; }

    public string DimensionId => _dimensionId;

    /// <summary>
    /// <para>When false, new load requests are rejected. Used during dimension switches.</para>
    /// </summary>
    public bool AcceptLoads { get; set; } = true;

    /// <summary>
    /// <para>Monotonic revision of the loaded chunk set and tile contents.</para>
    /// </summary>
    public long Revision { get; private set; }

    public ITileRegistry Tiles => _tiles;

    public TerrainGenerator TerrainGenerator => _terrainGenerator;

    public ChunkGenerator ChunkGenerator => _chunkGenerator;

    public int LoadedCount
    {
        get
        {
            lock (_tableLock)
            {
                return _chunks.Count;
            }
        }
    }

    /// <summary>
    /// <para>Try to get a loaded chunk.</para>
    /// </summary>
    public Chunk? GetChunk(ChunkVertexVector2D position)
    {
        lock (_tableLock)
        {
            return _chunks.GetValueOrDefault(position);
        }
    }

    /// <summary>
    /// <para>Get the loaded chunk containing a world cell.</para>
    /// </summary>
    public Chunk? GetChunkAt(TileVector2D worldCell) =>
        GetChunk(WorldConstants.GetChunkVertex(worldCell));

    /// <summary>
    /// <para>Load a chunk. Idempotent: a second call for a loaded chunk does nothing.</para>
    /// </summary>
    public bool LoadChunk(ChunkVertexVector2D position)
    {
        if (!AcceptLoads)
        {
            return false;
        }

        if ((position.X & WorldConstants.ChunkMask) != 0 ||
            (position.Y & WorldConstants.ChunkMask) != 0)
        {
            LogCat.Warn($"Ignored a chunk request that is not aligned to {WorldConstants.ChunkSize}: " +
                        $"{position.X},{position.Y}.");
            return false;
        }

        if (GetChunk(position) is not null)
        {
            return true;
        }

        var terrain = _terrainGenerator.GetOrGenerate(WorldConstants.GetTerrainVertex(position));
        if (terrain is null)
        {
            return false;
        }

        var saveData = _storage.Load(_dimensionId, position);
        Chunk chunk;
        var fromSave = saveData is not null;
        if (fromSave)
        {
            chunk = new Chunk(position, _tiles, _pool);
            chunk.Restore(saveData!);
        }
        else
        {
            chunk = _chunkGenerator.Generate(position, terrain);
        }

        chunk.Terrain = terrain;
        chunk.World = World;
        if (fromSave)
        {
            Growth?.CatchUp(chunk, World?.GlobalTick ?? 0);
        }

        chunk.AddReplaceCallback(OnTileChanged);

        lock (_tableLock)
        {
            if (_chunks.ContainsKey(position))
            {
                return true;
            }

            _chunks[position] = chunk;
        }

        Revision++;
        Light?.OnChunkLoaded(chunk);
        World?.OnChunkLoaded(chunk);

        if (fromSave)
        {
            var entities = _storage.LoadEntities(_dimensionId, position);
            World?.OnChunkEntitiesLoaded(chunk, entities);
        }

        return true;
    }

    /// <summary>
    /// <para>Unload and save a chunk. Returns false when saving failed; the chunk is kept in that case.</para>
    /// </summary>
    public bool UnloadChunk(ChunkVertexVector2D position)
    {
        var chunk = GetChunk(position);
        if (chunk is null)
        {
            return true;
        }

        // Invariant: clean tile entity bindings, then save, then clear light, then remove from the table.
        chunk.ClearTileEntityBindings();
        World?.BeforeChunkSave(chunk);
        var entities = World?.CollectChunkEntitiesForSave(chunk);

        if (!_storage.Save(_dimensionId, chunk.CreateSaveData()))
        {
            LogCat.Error($"Aborted unloading chunk {position.X},{position.Y}: save failed.");
            return false;
        }

        PersistEntities(position, entities);
        World?.AfterChunkSaved(chunk);
        Light?.ClearChunk(chunk);

        lock (_tableLock)
        {
            if (_chunks.TryGetValue(position, out var current) && ReferenceEquals(current, chunk))
            {
                _chunks.Remove(position);
            }
        }

        Revision++;
        World?.OnChunkUnloaded(chunk);
        return true;
    }

    /// <summary>
    /// <para>Save a loaded chunk without unloading it.</para>
    /// </summary>
    public bool SaveChunk(ChunkVertexVector2D position)
    {
        var chunk = GetChunk(position);
        if (chunk is null)
        {
            return true;
        }

        World?.BeforeChunkSave(chunk);
        var entities = World?.CollectChunkEntitiesForSave(chunk);
        var saved = _storage.Save(_dimensionId, chunk.CreateSaveData());
        if (saved)
        {
            PersistEntities(position, entities);
        }

        return saved;
    }

    /// <summary>
    /// <para>Snapshot of loaded chunk positions.</para>
    /// </summary>
    public List<ChunkVertexVector2D> GetLoadedPositions()
    {
        lock (_tableLock)
        {
            return _chunks.Keys.ToList();
        }
    }

    /// <summary>
    /// <para>Snapshot of loaded chunks.</para>
    /// </summary>
    public List<Chunk> GetLoadedChunks()
    {
        lock (_tableLock)
        {
            return _chunks.Values.ToList();
        }
    }

    /// <summary>
    /// <para>Save every loaded chunk. Returns false if any save failed.</para>
    /// </summary>
    public bool SaveAll()
    {
        var success = true;
        foreach (var position in GetLoadedPositions())
        {
            success &= SaveChunk(position);
        }

        return success;
    }

    /// <summary>
    /// <para>Unload every loaded chunk. Returns false if any unload was aborted.</para>
    /// </summary>
    public bool UnloadAll()
    {
        var success = true;
        foreach (var position in GetLoadedPositions())
        {
            success &= UnloadChunk(position);
        }

        return success;
    }

    private void PersistEntities(ChunkVertexVector2D position, Save.ChunkEntityMessage? entities)
    {
        if (entities is null || entities.Entities.Count == 0)
        {
            _storage.DeleteEntities(_dimensionId, position);
            return;
        }

        _storage.SaveEntities(_dimensionId, position, entities);
    }

    private void OnTileChanged(Chunk chunk, int layer, int index, Tile? oldTile, Tile? newTile,
        TileDestroySource destroySource)
    {
        Revision++;
        Light?.OnChunkChanged(chunk, layer, index, oldTile, newTile, destroySource);

        // Generation, loading and unloading do not cascade attachments.
        if (destroySource is TileDestroySource.WorldGeneration or TileDestroySource.Load
            or TileDestroySource.Unload)
        {
            return;
        }

        Attachments?.OnCellChanged(chunk, layer, index, destroySource);
    }
}
