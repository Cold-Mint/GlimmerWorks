using Arch.Core;
using ArchEntity = Arch.Core.Entity;
using ArchWorld = Arch.Core.World;

namespace GlimmerWorks.Engine.Ecs;

/// <summary>
/// <para>EcsWorld</para>
/// <para>对 Arch 实体存储的薄封装，隔离第三方 API 并集中处理实体生命周期。</para>
/// </summary>
/// <remarks>
/// <para>每个维度拥有一个实例：卸载维度时整体销毁，避免跨维度查询与悬挂实体。</para>
/// <para>One store per dimension. Disposing the dimension destroys every entity with it.</para>
/// <para>注意：<c>Arch.Core.World</c> 与本项目的 <c>GlimmerWorks.Engine.World.World</c> 同名，本文件使用别名。</para>
/// </remarks>
public sealed class EcsWorld : IDisposable
{
    private readonly ArchWorld _store;
    private bool _disposed;

    public EcsWorld()
    {
        _store = ArchWorld.Create();
    }

    /// <summary>
    /// <para>Escape hatch for advanced Arch usage. Prefer the methods on this class.</para>
    /// </summary>
    public ArchWorld Store => _store;

    public bool IsDisposed => _disposed;

    public ArchEntity Create() => _store.Create();

    public ArchEntity Create<T0>(T0 component)
    {
        return _store.Create(in component);
    }

    public ArchEntity Create<T0, T1>(T0 c0, T1 c1)
    {
        return _store.Create(in c0, in c1);
    }

    public ArchEntity Create<T0, T1, T2>(T0 c0, T1 c1, T2 c2)
    {
        return _store.Create(in c0, in c1, in c2);
    }

    public ArchEntity Create<T0, T1, T2, T3>(T0 c0, T1 c1, T2 c2, T3 c3)
    {
        return _store.Create(in c0, in c1, in c2, in c3);
    }

    public ArchEntity Create<T0, T1, T2, T3, T4>(T0 c0, T1 c1, T2 c2, T3 c3, T4 c4)
    {
        return _store.Create(in c0, in c1, in c2, in c3, in c4);
    }

    public bool IsAlive(ArchEntity entity) => !_disposed && _store.IsAlive(entity);

    public bool Has<T>(ArchEntity entity) => _store.Has<T>(entity);

    public ref T Get<T>(ArchEntity entity) => ref _store.Get<T>(entity);

    public bool TryGet<T>(ArchEntity entity, out T component) where T : notnull
    {
        if (_store.TryGet<T>(entity, out var value))
        {
            component = value!;
            return true;
        }

        component = default!;
        return false;
    }

    public void Add<T>(ArchEntity entity, T component)
    {
        _store.Add(entity, in component);
    }

    public void Remove<T>(ArchEntity entity) => _store.Remove<T>(entity);

    public void Destroy(ArchEntity entity)
    {
        if (_store.IsAlive(entity))
        {
            _store.Destroy(entity);
        }
    }

    public int Count<T>()
    {
        var description = new QueryDescription().WithAll<T>();
        return _store.CountEntities(in description);
    }

    public void ForEach<T0>(in QueryDescription description, ForEachWithEntity<T0> action)
    {
        _store.Query(in description, action);
    }

    public void ForEach<T0, T1>(in QueryDescription description, ForEachWithEntity<T0, T1> action)
    {
        _store.Query(in description, action);
    }

    public void ForEach<T0, T1, T2>(in QueryDescription description, ForEachWithEntity<T0, T1, T2> action)
    {
        _store.Query(in description, action);
    }

    public void ForEach<T0, T1, T2, T3>(in QueryDescription description,
        ForEachWithEntity<T0, T1, T2, T3> action)
    {
        _store.Query(in description, action);
    }

    /// <summary>
    /// <para>Collect entities matching a query into a list. Used before structural changes (destroy).</para>
    /// </summary>
    public List<ArchEntity> Collect<T0>()
    {
        var description = new QueryDescription().WithAll<T0>();
        var entities = new List<ArchEntity>();
        _store.Query(in description, (ArchEntity entity) => entities.Add(entity));
        return entities;
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        _disposed = true;
        _store.Dispose();
    }
}
