using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Terrain;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>ChunkTileChanged</para>
/// </summary>
public delegate void ChunkTileChanged(
    Chunk chunk,
    int layer,
    int index,
    Tile? oldTile,
    Tile? newTile,
    TileDestroySource destroySource);

/// <summary>
/// <para>Chunk</para>
/// </summary>
/// <remarks>
/// <para>Arrays for a layer are allocated lazily the first time that layer is written.</para>
/// </remarks>
public sealed class Chunk
{
    private readonly Tile?[]?[] _tiles = new Tile?[]?[WorldConstants.LayerCount];
    private readonly ulong[]?[] _fingerprints = new ulong[]?[WorldConstants.LayerCount];
    private readonly TileState?[]?[] _states = new TileState?[]?[WorldConstants.LayerCount];
    private readonly TileSnapshot?[]?[] _snapshots = new TileSnapshot?[]?[WorldConstants.LayerCount];
    private readonly List<ChunkTileChanged> _replaceCallbacks = [];
    private readonly ITileRegistry _registry;
    private readonly TileInstancePool _pool;

    public Chunk(ChunkVertexVector2D position, ITileRegistry registry, TileInstancePool pool)
    {
        Position = position;
        _registry = registry;
        _pool = pool;
    }

    /// <summary>
    /// <para>Bottom-left chunk vertex.</para>
    /// </summary>
    public ChunkVertexVector2D Position { get; }

    /// <summary>
    /// <para>Strong reference to the terrain block, keeping it alive while this chunk exists.</para>
    /// </summary>
    public TerrainBlock? Terrain { get; set; }

    /// <summary>
    /// <para>The world that owns this chunk. May be null while generating or loading.</para>
    /// </summary>
    public GlimmerWorks.Engine.World.World? World { get; set; }

    /// <summary>
    /// <para>Monotonic revision used by caches.</para>
    /// </summary>
    public long Revision { get; private set; }

    /// <summary>
    /// <para>Whether the layer has allocated arrays.</para>
    /// </summary>
    public bool HasLayer(int layerIndex) => layerIndex is >= 0 and < WorldConstants.LayerCount && _states[layerIndex] != null;

    /// <summary>
    /// <para>Number of allocated layers.</para>
    /// </summary>
    public int AllocatedLayerCount => _states.Count(static layer => layer != null);

    /// <summary>
    /// <para>Get the authoritative state of a cell.</para>
    /// </summary>
    public TileState? GetState(int layerIndex, int index)
    {
        if (!IsValid(layerIndex, index))
        {
            return null;
        }

        return _states[layerIndex]?[index];
    }

    /// <summary>
    /// <para>Get the shared tile instance of a cell.</para>
    /// </summary>
    public Tile? GetTile(int layerIndex, int index)
    {
        if (!IsValid(layerIndex, index))
        {
            return null;
        }

        return _tiles[layerIndex]?[index];
    }

    /// <summary>
    /// <para>Get the render snapshot of a cell.</para>
    /// </summary>
    public TileSnapshot? GetSnapshot(int layerIndex, int index)
    {
        if (!IsValid(layerIndex, index))
        {
            return null;
        }

        return _snapshots[layerIndex]?[index];
    }

    /// <summary>
    /// <para>Get the resource fingerprint of a cell.</para>
    /// </summary>
    public ulong GetFingerprint(int layerIndex, int index)
    {
        if (!IsValid(layerIndex, index))
        {
            return 0;
        }

        return _fingerprints[layerIndex]?[index] ?? 0;
    }

    /// <summary>
    /// <para>Add a callback invoked whenever a cell's tile instance changes.</para>
    /// </summary>
    public void AddReplaceCallback(ChunkTileChanged callback)
    {
        _replaceCallbacks.Add(callback);
    }

    public void RemoveReplaceCallback(ChunkTileChanged callback)
    {
        _replaceCallbacks.Remove(callback);
    }

    /// <summary>
    /// <para>Try to place a multi-tile tile inside this chunk. The top-left index must be inside the chunk.</para>
    /// </summary>
    public bool Place(int layer, int topLeftIndex, ResourceRef resourceRef, TileResource resource,
        TileDestroySource destroySource, TilePlaceSource placeSource)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0 || topLeftIndex is < 0 or >= WorldConstants.ChunkCellCount)
        {
            LogCat.Warn($"Invalid tile placement (layer={layer}, topLeftIndex={topLeftIndex}).");
            return false;
        }

        var width = resource.ClampedWidth;
        var height = resource.ClampedHeight;
        var left = WorldConstants.ChunkIndexX(topLeftIndex);
        var top = WorldConstants.ChunkIndexY(topLeftIndex);
        if (left + width > WorldConstants.ChunkSize ||
            top - (height - 1) < 0)
        {
            LogCat.Warn(
                $"Multi-tile placement out of chunk bounds (topLeft={left},{top},{width}={width},height={height}).");
            return false;
        }

        for (var cellX = 0; cellX < width; cellX++)
        {
            for (var cellY = 0; cellY < height; cellY++)
            {
                var index = topLeftIndex + cellX - cellY * WorldConstants.ChunkSize;
                if (!WriteSingle(layerIndex, index, resourceRef, resource, cellX, cellY, destroySource, placeSource))
                {
                    return false;
                }
            }
        }

        return true;
    }

    /// <summary>
    /// <para>Write one cell. Used by in-chunk placement and by the cross-chunk placer.</para>
    /// </summary>
    public bool WriteSingle(int layerIndex, int index, ResourceRef resourceRef, TileResource resource,
        int cellX, int cellY, TileDestroySource destroySource, TilePlaceSource placeSource)
    {
        if (!IsValid(layerIndex, index))
        {
            return false;
        }

        EnsureLayer(layerIndex);
        var replacedState = _states[layerIndex]![index];
        var state = new TileState
        {
            Width = resource.ClampedWidth,
            Height = resource.ClampedHeight,
            PlaceSource = placeSource,
            Offset = new TileVector2D(-cellX, cellY),
            ResourceRef = resourceRef
        };

        InitializeGrowth(state, resource, layerIndex, index);
        InitializeContainer(state, resource, cellX, cellY);

        _states[layerIndex]![index] = state;
        if (Commit(layerIndex, index, destroySource, replacedState))
        {
            return true;
        }

        // Resource lookup failed: restore the authoritative state so the cell is not left half-written.
        _states[layerIndex]![index] = replacedState;
        return false;
    }

    /// <summary>
    /// <para>Commit a cell's state, rebuilding the tile instance when the resource fingerprint changed.</para>
    /// </summary>
    public bool Commit(int layerIndex, int index, TileDestroySource destroySource,
        TileState? replacedState = null)
    {
        if (!IsValid(layerIndex, index))
        {
            return false;
        }

        var states = _states[layerIndex];
        var state = states?[index];
        var oldTile = _tiles[layerIndex]?[index];

        if (state is null)
        {
            RemoveInstance(layerIndex, index, oldTile, replacedState, destroySource);
            return true;
        }

        var fingerprint = ResourceFingerprint.Of(state.ResourceRef);
        var fingerprints = _fingerprints[layerIndex];
        var sameFingerprint = fingerprints is not null && fingerprints[index] == fingerprint;
        if (sameFingerprint && oldTile is not null)
        {
            // State-only changes (container contents, growth progress) do not rebuild the instance.
            EnsureLayer(layerIndex);
            _snapshots[layerIndex]![index] = new TileSnapshot(oldTile, state);
            Touch();
            return true;
        }

        var resource = _registry.Find(state.ResourceRef);
        if (resource is null)
        {
            LogCat.Warn(
                $"Tile resource not found while committing (layer={WorldConstants.IndexToLayer(layerIndex)}, index={index}, ref={state.ResourceRef.TargetPackId}:{state.ResourceRef.ResourceId}).");
            return false;
        }

        var newTile = _pool.GetOrCreate(state.ResourceRef, resource, fingerprint);
        if (newTile is null)
        {
            return false;
        }

        EnsureLayer(layerIndex);
        if (oldTile is not null && !ReferenceEquals(oldTile, newTile))
        {
            var oldCell = GetWorldCell(index);
            var oldState = replacedState ?? state;
            oldTile.OnReplaced(this, WorldConstants.IndexToLayer(layerIndex), index, oldCell, oldState,
                destroySource);
        }

        _tiles[layerIndex]![index] = newTile;
        _fingerprints[layerIndex]![index] = fingerprint;
        _snapshots[layerIndex]![index] = new TileSnapshot(newTile, state);
        _states[layerIndex]![index] = state;

        var cell = GetWorldCell(index);
        newTile.OnPlaced(this, WorldConstants.IndexToLayer(layerIndex), index, cell, state, state.PlaceSource);
        NotifyChanged(layerIndex, index, oldTile, newTile, destroySource);
        Touch();
        return true;
    }

    /// <summary>
    /// <para>Clear a cell and remove its instance.</para>
    /// </summary>
    public bool ClearCell(int layerIndex, int index, TileDestroySource destroySource)
    {
        if (!IsValid(layerIndex, index))
        {
            return false;
        }

        EnsureLayer(layerIndex);
        var oldTile = _tiles[layerIndex]?[index];
        var oldState = _states[layerIndex]?[index];
        _states[layerIndex]![index] = null;
        RemoveInstance(layerIndex, index, oldTile, oldState, destroySource);
        return true;
    }

    /// <summary>
    /// <para>Break every cell covered by the multi-tile at the given top-left index.</para>
    /// </summary>
    public bool BreakTile(int layerIndex, int topLeftIndex, TileDestroySource destroySource)
    {
        if (!IsValid(layerIndex, topLeftIndex))
        {
            return false;
        }

        var state = _states[layerIndex]?[topLeftIndex];
        if (state is null)
        {
            return ClearCell(layerIndex, topLeftIndex, destroySource);
        }

        var width = Math.Clamp(state.Width, 1, WorldConstants.ChunkSize);
        var height = Math.Clamp(state.Height, 1, WorldConstants.ChunkSize);
        var left = WorldConstants.ChunkIndexX(topLeftIndex);
        var top = WorldConstants.ChunkIndexY(topLeftIndex);
        for (var cellX = 0; cellX < width; cellX++)
        {
            for (var cellY = 0; cellY < height; cellY++)
            {
                var index = WorldConstants.ChunkIndex(left + cellX, top - cellY);
                ClearCell(layerIndex, index, destroySource);
            }
        }

        return true;
    }

    /// <summary>
    /// <para>Create a save DTO. Every layer contains 256 slots, empty slots included.</para>
    /// </summary>
    public ChunkSaveData CreateSaveData()
    {
        var data = new ChunkSaveData { X = Position.X, Y = Position.Y };
        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            var layer = new ChunkLayerSaveData
            {
                Layer = WorldConstants.IndexToLayer(layerIndex),
                States = new List<TileStateSaveData?>(WorldConstants.ChunkCellCount)
            };
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var state = _states[layerIndex]?[index];
                layer.States.Add(state is null ? null : TileStateSerialization.ToSaveData(state));
            }

            data.Layers.Add(layer);
        }

        return data;
    }

    /// <summary>
    /// <para>Restore states from save data and rebuild instances.</para>
    /// </summary>
    public void Restore(ChunkSaveData data)
    {
        foreach (var layer in data.Layers)
        {
            var layerIndex = WorldConstants.LayerToIndex(layer.Layer);
            if (layerIndex < 0)
            {
                continue;
            }

            EnsureLayer(layerIndex);
            for (var index = 0; index < WorldConstants.ChunkCellCount && index < layer.States.Count; index++)
            {
                var saveState = layer.States[index];
                if (saveState is null)
                {
                    continue;
                }

                var state = TileStateSerialization.FromSaveData(saveState);
                if (!state.IsOriginCell)
                {
                    // Only the bottom-left origin cell may own a uuid or container.
                    state.Uuid = null;
                    state.Container = null;
                }

                _states[layerIndex]![index] = state;
            }
        }

        NormalizeRestoredMultiTiles();

        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            if (_states[layerIndex] is null)
            {
                continue;
            }

            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                if (_states[layerIndex]![index] is not null)
                {
                    Commit(layerIndex, index, TileDestroySource.Load);
                }
            }
        }
    }

    /// <summary>
    /// <para>Repair the common "same top-left, different width/height/resource" corruption in restored saves.</para>
    /// </summary>
    private void NormalizeRestoredMultiTiles()
    {
        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            var states = _states[layerIndex];
            if (states is null)
            {
                continue;
            }

            var groups = new Dictionary<(int X, int Y), List<(int Index, TileState State)>>();
            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var state = states[index];
                if (state is null)
                {
                    continue;
                }

                var topLeft = state.GetTopLeft(GetWorldCell(index));
                var key = (topLeft.X, topLeft.Y);
                if (!groups.TryGetValue(key, out var group))
                {
                    group = [];
                    groups[key] = group;
                }

                group.Add((index, state));
            }

            foreach (var group in groups.Values)
            {
                TileState? reference = null;
                foreach (var entry in group)
                {
                    if (entry.State.IsOriginCell)
                    {
                        reference = entry.State;
                        break;
                    }
                }

                reference ??= group[0].State;
                if (reference is null)
                {
                    continue;
                }

                foreach (var (index, state) in group)
                {
                    if (ReferenceEquals(state, reference))
                    {
                        continue;
                    }

                    state.Width = reference.Width;
                    state.Height = reference.Height;
                    if (!SameResource(state.ResourceRef, reference.ResourceRef))
                    {
                        state.ResourceRef = CloneResourceRef(reference.ResourceRef);
                    }

                    if (!state.IsOriginCell)
                    {
                        state.Uuid = null;
                        state.Container = null;
                    }

                    states[index] = state;
                }
            }
        }
    }

    private static bool SameResource(ResourceRef left, ResourceRef right) =>
        left.TargetPackId == right.TargetPackId &&
        left.ResourceType == right.ResourceType &&
        left.ResourceId == right.ResourceId;

    private static ResourceRef CloneResourceRef(ResourceRef source)
    {
        var clone = new ResourceRef
        {
            TargetPackId = source.TargetPackId,
            ResourceType = source.ResourceType,
            ResourceId = source.ResourceId
        };
        clone.SetPackId(string.IsNullOrEmpty(source.RequesterPackId)
            ? source.TargetPackId
            : source.RequesterPackId);
        return clone;
    }

    /// <summary>
    /// <para>Invoke <see cref="Tile.OnUnloaded"/> for every occupied cell. Custom tiles clear external bindings here.</para>
    /// </summary>
    public void ClearTileEntityBindings()
    {
        ForEachOccupied((layerIndex, index, tile, state) =>
        {
            tile.OnUnloaded(this, WorldConstants.IndexToLayer(layerIndex), index, GetWorldCell(index), state);
        });
    }

    /// <summary>
    /// <para>Iterate a copy of all occupied cells so callbacks may mutate the chunk.</para>
    /// </summary>
    public void ForEachOccupied(Action<int, int, Tile, TileState> action)
    {
        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            var tiles = _tiles[layerIndex];
            var states = _states[layerIndex];
            if (tiles is null || states is null)
            {
                continue;
            }

            for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
            {
                var tile = tiles[index];
                var state = states[index];
                if (tile is not null && state is not null)
                {
                    action(layerIndex, index, tile, state);
                }
            }
        }
    }

    /// <summary>
    /// <para>Iterate the states of a layer, including empty slots.</para>
    /// </summary>
    public IEnumerable<(int Index, TileState? State)> EnumerateLayer(int layerIndex)
    {
        if (layerIndex is < 0 or >= WorldConstants.LayerCount)
        {
            yield break;
        }

        for (var index = 0; index < WorldConstants.ChunkCellCount; index++)
        {
            yield return (index, _states[layerIndex]?[index]);
        }
    }

    /// <summary>
    /// <para>World cell coordinate of a chunk-local index.</para>
    /// </summary>
    public TileVector2D GetWorldCell(int index) => new(
        Position.X + WorldConstants.ChunkIndexX(index),
        Position.Y + WorldConstants.ChunkIndexY(index));

    /// <summary>
    /// <para>Whether a world cell belongs to this chunk.</para>
    /// </summary>
    public bool Contains(TileVector2D cell) =>
        cell.X >= Position.X && cell.X < Position.X + WorldConstants.ChunkSize &&
        cell.Y >= Position.Y && cell.Y < Position.Y + WorldConstants.ChunkSize;

    /// <summary>
    /// <para>Chunk-local index of a world cell, or -1 when outside.</para>
    /// </summary>
    public int IndexOf(TileVector2D cell)
    {
        if (!Contains(cell))
        {
            return -1;
        }

        return WorldConstants.ChunkIndex(cell.X - Position.X, cell.Y - Position.Y);
    }

    private void EnsureLayer(int layerIndex)
    {
        if (_states[layerIndex] is not null)
        {
            return;
        }

        _tiles[layerIndex] = new Tile?[WorldConstants.ChunkCellCount];
        _fingerprints[layerIndex] = new ulong[WorldConstants.ChunkCellCount];
        _states[layerIndex] = new TileState?[WorldConstants.ChunkCellCount];
        _snapshots[layerIndex] = new TileSnapshot?[WorldConstants.ChunkCellCount];
    }

    private void RemoveInstance(int layerIndex, int index, Tile? oldTile, TileState? oldState,
        TileDestroySource destroySource)
    {
        if (oldTile is not null)
        {
            oldTile.OnReplaced(this, WorldConstants.IndexToLayer(layerIndex), index, GetWorldCell(index),
                oldState ?? new TileState(), destroySource);
        }

        if (_tiles[layerIndex] is not null)
        {
            _tiles[layerIndex]![index] = null;
        }

        if (_fingerprints[layerIndex] is not null)
        {
            _fingerprints[layerIndex]![index] = 0;
        }

        if (_snapshots[layerIndex] is not null)
        {
            _snapshots[layerIndex]![index] = null;
        }

        NotifyChanged(layerIndex, index, oldTile, null, destroySource);
        Touch();
    }

    private void NotifyChanged(int layerIndex, int index, Tile? oldTile, Tile? newTile, TileDestroySource source)
    {
        if (_replaceCallbacks.Count == 0)
        {
            return;
        }

        var layer = WorldConstants.IndexToLayer(layerIndex);
        foreach (var callback in _replaceCallbacks.ToArray())
        {
            callback(this, layer, index, oldTile, newTile, source);
        }
    }

    private void InitializeGrowth(TileState state, TileResource resource, int layerIndex, int index)
    {
        var currentTick = World?.GlobalTick ?? 0;
        state.TilePlacedTick = currentTick;
        state.TileGrowthStartTick = currentTick;
        state.MatureCount = 0;
        state.GrowthConditionsMet = false;

        if (resource.GrowthMinTicks <= 0)
        {
            state.GrowthTicks = 0;
            state.GrowthDurationTicks = 0;
            state.GrowthMature = false;
            return;
        }

        var min = resource.GrowthMinTicks;
        var max = Math.Max(resource.GrowthMaxTicks, min);
        var range = max - min;
        var salt = WorldHash.Combine(Position.X, Position.Y, layerIndex, index,
            (long)ResourceFingerprint.Of(state.ResourceRef));
        state.GrowthTicks = 0;
        state.GrowthDurationTicks = range <= 0 ? min : min + (long)(salt % (ulong)(range + 1));
        state.GrowthMature = false;
    }

    private void InitializeContainer(TileState state, TileResource resource, int cellX, int cellY)
    {
        // The bottom-left origin cell is cellY = height - 1.
        var origin = cellX == 0 && cellY == state.Height - 1;

        // Only the origin cell owns the container and its loot identity.
        if (origin && resource.ContainerCapacity > 0)
        {
            state.Container = new ContainerState
            {
                Capacity = resource.ContainerCapacity,
                MaxStackPerSlot = Math.Max(1, resource.MaxStackPerSlot)
            };
            state.Container.EnsureSlots();
        }
        else
        {
            state.Container = null;
        }

        // Loot-box identity is only assigned to containers at the origin.
        state.Uuid = origin && resource.ContainerCapacity > 0 ? Guid.NewGuid() : null;
    }

    private void Touch()
    {
        Revision++;
    }

    private static bool IsValid(int layerIndex, int index) =>
        layerIndex is >= 0 and < WorldConstants.LayerCount &&
        index is >= 0 and < WorldConstants.ChunkCellCount;
}

/// <summary>
/// <para>WorldHash</para>
/// </summary>
public static class WorldHash
{
    public static ulong Combine(params long[] values)
    {
        var hash = 14695981039346656037UL;
        foreach (var value in values)
        {
            var bits = unchecked((ulong)value);
            for (var i = 0; i < 8; i++)
            {
                hash ^= (byte)(bits >> (i * 8));
                hash *= 1099511628211UL;
            }
        }

        return hash;
    }
}
