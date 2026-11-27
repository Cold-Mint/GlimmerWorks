using Arch.Core;
using GlimmerWorks.Engine.Ecs;
using GlimmerWorks.Engine.Ecs.Components;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.World.Save;
using MemoryPack;
using ArchEntity = Arch.Core.Entity;

namespace GlimmerWorks.Engine.World.Entities;

/// <summary>
/// <para>ItemEntityStore</para>
/// <para>一个维度内掉落物实体的创建、按区块收集、读档恢复与销毁。</para>
/// </summary>
/// <remarks>
/// <para>不依赖 <see cref="World"/>，便于单独测试；维度差异由传入的 <see cref="EcsWorld"/> 与 id 分配器表达。</para>
/// <para>实体与区块的关联通过 <see cref="ChunkAnchor"/> 表达，不要求区块当前已加载。</para>
/// </remarks>
public sealed class ItemEntityStore
{
    private const int ItemDropComponentType = 1;

    private readonly EcsWorld _ecs;
    private readonly Func<ulong> _allocateId;
    private readonly Action<ulong> _observeId;

    public ItemEntityStore(EcsWorld ecs, Func<ulong> allocateId, Action<ulong> observeId)
    {
        _ecs = ecs;
        _allocateId = allocateId;
        _observeId = observeId;
    }

    /// <summary>
    /// <para>Create one dropped-item entity anchored to a chunk.</para>
    /// </summary>
    public ArchEntity Spawn(ResourceRef item, int count, int cellX, int cellY, int chunkX, int chunkY)
    {
        return _ecs.Create(
            new PersistentId { Value = _allocateId() },
            new ItemDropComponent { Item = item, Count = Math.Max(1, count) },
            new CellPosition { X = cellX, Y = cellY },
            new ChunkAnchor { X = chunkX, Y = chunkY });
    }

    /// <summary>
    /// <para>Serialize every entity anchored to the chunk. Returns null when the chunk has none.</para>
    /// </summary>
    public ChunkEntityMessage? Collect(int chunkX, int chunkY)
    {
        var message = new ChunkEntityMessage();
        var query = new QueryDescription()
            .WithAll<ChunkAnchor, PersistentId, ItemDropComponent, CellPosition>();

        _ecs.ForEach<ChunkAnchor, PersistentId, ItemDropComponent, CellPosition>(
            in query,
            (ArchEntity entity, ref ChunkAnchor anchor, ref PersistentId id, ref ItemDropComponent item,
                ref CellPosition cell) =>
            {
                if (anchor.X != chunkX || anchor.Y != chunkY)
                {
                    return;
                }

                var payload = new ItemDropMessage
                {
                    Count = item.Count,
                    Cell = new Vector2DMessage { X = cell.X, Y = cell.Y }
                };

                message.Entities.Add(new EntityItemMessage
                {
                    GameEntity = new GameEntityMessage { Id = id.Value },
                    ResourceRef = SaveMessageMapper.ToMessage(item.Item),
                    Components =
                    {
                        new ComponentMessage
                        {
                            Type = ItemDropComponentType,
                            Data = MemoryPackSerializer.Serialize(payload)
                        }
                    }
                });
            });

        return message.Entities.Count == 0 ? null : message;
    }

    /// <summary>
    /// <para>Recreate entities saved for a chunk.</para>
    /// </summary>
    public void Load(int chunkX, int chunkY, ChunkEntityMessage? message)
    {
        if (message is null || message.Entities.Count == 0)
        {
            return;
        }

        foreach (var saved in message.Entities)
        {
            var item = SaveMessageMapper.ToResourceRef(saved.ResourceRef);
            if (string.IsNullOrEmpty(item.ResourceId))
            {
                continue;
            }

            var count = 1;
            var cellX = 0;
            var cellY = 0;
            foreach (var component in saved.Components)
            {
                if (component.Type != ItemDropComponentType)
                {
                    continue;
                }

                try
                {
                    var payload = MemoryPackSerializer.Deserialize<ItemDropMessage>(component.Data);
                    if (payload is null)
                    {
                        continue;
                    }

                    count = Math.Max(1, payload.Count);
                    cellX = payload.Cell.X;
                    cellY = payload.Cell.Y;
                }
                catch (Exception exception)
                {
                    LogCat.Warn($"Skipped malformed item drop component: {exception.Message}");
                }
            }

            var id = saved.GameEntity.Id;
            if (id == 0)
            {
                id = _allocateId();
            }
            else
            {
                _observeId(id);
            }

            _ecs.Create(
                new PersistentId { Value = id },
                new ItemDropComponent { Item = item, Count = count },
                new CellPosition { X = cellX, Y = cellY },
                new ChunkAnchor { X = chunkX, Y = chunkY });
        }
    }

    /// <summary>
    /// <para>Destroy entities of a chunk, after its save succeeded.</para>
    /// </summary>
    public void Remove(int chunkX, int chunkY)
    {
        var entities = _ecs.Collect<ChunkAnchor>();
        foreach (var entity in entities)
        {
            if (!_ecs.TryGet<ChunkAnchor>(entity, out var anchor))
            {
                continue;
            }

            if (anchor.X == chunkX && anchor.Y == chunkY)
            {
                _ecs.Destroy(entity);
            }
        }
    }
}
