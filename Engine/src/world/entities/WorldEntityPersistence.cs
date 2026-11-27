using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.World.Save;
using Chunk = GlimmerWorks.Engine.World.Tiles.Chunk;

namespace GlimmerWorks.Engine.World.Entities;

/// <summary>
/// <para>WorldEntityPersistence</para>
/// <para>把维度的动态实体接到现有的区块实体存档管线（entities.bin）。</para>
/// </summary>
/// <remarks>
/// <para>实体只保存“属于哪个区块、在哪个格子、有哪些组件”，瓦片本身仍由 <see cref="Chunk"/> 的连续数组保存。</para>
/// <para>每个维度一个 <see cref="ItemEntityStore"/>，维度销毁时随 <c>EcsWorld</c> 一起释放。</para>
/// </remarks>
public sealed class WorldEntityPersistence : IWorldChunkEntityPersistence
{
    private readonly World _world;
    private readonly Dictionary<DimensionRuntime, ItemEntityStore> _stores = [];

    public WorldEntityPersistence(World world)
    {
        _world = world;
    }

    /// <summary>
    /// <para>Create a dropped-item entity in the dimension that owns the chunk.</para>
    /// </summary>
    public void SpawnItemDrop(Chunk chunk, ResourceRef item, int count, TileVector2D cell)
    {
        if (count <= 0 || string.IsNullOrEmpty(item.ResourceId))
        {
            return;
        }

        GetStore(chunk)?.Spawn(item, count, cell.X, cell.Y, chunk.Position.X, chunk.Position.Y);
    }

    /// <summary>
    /// <para>Item dropped event handler, subscribed by <see cref="World"/>.</para>
    /// </summary>
    public void OnItemDropped(Chunk chunk, ResourceRef item, int count, TileVector2D cell)
    {
        SpawnItemDrop(chunk, item, count, cell);
    }

    /// <summary>
    /// <para>Serialize entities anchored to the chunk, called before the chunk is written.</para>
    /// </summary>
    public ChunkEntityMessage? CollectChunkEntities(Chunk chunk)
    {
        return GetStore(chunk)?.Collect(chunk.Position.X, chunk.Position.Y);
    }

    /// <summary>
    /// <para>Recreate entities stored for a chunk that just loaded.</para>
    /// </summary>
    public void LoadChunkEntities(Chunk chunk, ChunkEntityMessage? message)
    {
        GetStore(chunk)?.Load(chunk.Position.X, chunk.Position.Y, message);
    }

    /// <summary>
    /// <para>Containers still live in <c>TileState</c>; implement the entity-side copy when containers become entities.</para>
    /// </summary>
    public void SyncContainersToTileStates(Chunk chunk)
    {
    }

    /// <summary>
    /// <para>Destroy entities of a chunk after it was saved successfully.</para>
    /// </summary>
    public void SaveAndRemoveChunkEntities(Chunk chunk)
    {
        GetStore(chunk)?.Remove(chunk.Position.X, chunk.Position.Y);
    }

    private ItemEntityStore? GetStore(Chunk chunk)
    {
        var dimension = ResolveDimension(chunk);
        if (dimension is null)
        {
            return null;
        }

        if (_stores.TryGetValue(dimension, out var store))
        {
            return store;
        }

        store = new ItemEntityStore(dimension.Ecs, () => _world.AllocateEntityId(), _world.ObserveEntityId);
        _stores[dimension] = store;
        return store;
    }

    private DimensionRuntime? ResolveDimension(Chunk chunk)
    {
        if (!string.IsNullOrEmpty(chunk.DimensionId) &&
            _world.Dimensions.TryGetValue(chunk.DimensionId, out var byId))
        {
            return byId;
        }

        foreach (var dimension in _world.Dimensions.Values)
        {
            if (ReferenceEquals(dimension.Store.GetChunk(chunk.Position), chunk))
            {
                return dimension;
            }
        }

        return null;
    }
}
