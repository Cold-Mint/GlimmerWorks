using GlimmerWorks.Engine.World.Ecs;
using ArchWorld = Arch.Core.World;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>EntityManager</para>
/// <para>实体管理器。负责创建/销毁实体，以及挂载、读取和移除组件。</para>
/// </summary>
/// <remarks>
/// <para>每个世界持有独立实例，内部使用 Arch 存储组件，查询时按组件组合连续遍历。</para>
/// <para>One instance per world. Components are stored by Arch, so entities with the same components are iterated together.</para>
/// <para>注意：Add/Remove/Destroy 都会改变实体的组件组合（Arch 中的结构变更），不要在遍历查询的过程中调用。</para>
/// </remarks>
public sealed class EntityManager : IDisposable
{
    private readonly ArchWorld _world = ArchWorld.Create();

    /// <summary>
    /// <para>Live component counts, keyed by component type. Updated on every structural change.</para>
    /// <para>组件实时数量，以组件类型为键；每次结构变更都会同步更新。</para>
    /// </summary>
    private readonly Dictionary<Type, int> _componentCounts = new();

    private bool _disposed;

    /// <summary>
    /// <para>Number of alive entities.</para>
    /// <para>当前存活实体数量。</para>
    /// </summary>
    public int EntityCount => _world.Size;

    /// <summary>
    /// <para>Live component counts. Do not modify; the dictionary changes when entities change.</para>
    /// <para>组件的实时数量；请不要修改，实体的结构变更会同步更新它。</para>
    /// </summary>
    public IReadOnlyDictionary<Type, int> ComponentCounts => _componentCounts;

    /// <summary>
    /// <para>Count entities that currently have the component <typeparamref name="T"/>.</para>
    /// <para>统计当前挂载了 <typeparamref name="T"/> 组件的实体数量。</para>
    /// </summary>
    /// <remarks>
    /// <para>An entity can have at most one component of a type, so this is also the number of that component.</para>
    /// <para>一个实体最多拥有一个同类型组件，因此结果也可以看作该组件的数量。</para>
    /// </remarks>
    public int CountComponents<T>() where T : struct, IComponent
    {
        ThrowIfDisposed();
        return _componentCounts.GetValueOrDefault(typeof(T));
    }

    public int CountComponents(Type type)
    {
        ThrowIfDisposed();
        return _componentCounts.GetValueOrDefault(type);
    }

    /// <summary>
    /// <para>Create an entity without any component.</para>
    /// <para>创建一个没有任何组件的实体。</para>
    /// </summary>
    /// <returns>
    /// <para>The new entity handle. Attach components with <see cref="AddComponent{T}"/>.</para>
    /// <para>新实体句柄。之后用 <see cref="AddComponent{T}"/> 挂载组件。</para>
    /// </returns>
    public Entity CreateEntity()
    {
        ThrowIfDisposed();
        return new Entity(_world.Create());
    }

    /// <summary>
    /// <para>Create an entity and attach one component in a single call.</para>
    /// <para>创建实体并同时挂载一个组件。</para>
    /// </summary>
    public Entity CreateEntity<T>(T component) where T : struct, IComponent
    {
        ThrowIfDisposed();
        var entity = new Entity(_world.Create(in component));
        IncreaseComponentCount(typeof(T));
        return entity;
    }

    /// <summary>
    /// <para>Whether the entity is still alive in this manager.</para>
    /// <para>实体是否仍存活于当前管理器。</para>
    /// </summary>
    public bool IsAlive(Entity entity)
    {
        ThrowIfDisposed();
        return entity.Handle.WorldId == _world.Id && _world.IsAlive(entity.Handle);
    }

    /// <summary>
    /// <para>Whether the entity has a component of type <typeparamref name="T"/>.</para>
    /// <para>实体是否挂载了 <typeparamref name="T"/> 组件。</para>
    /// </summary>
    public bool HasComponent<T>(Entity entity) where T : struct, IComponent
    {
        EnsureOwned(entity);
        return _world.Has<T>(entity.Handle);
    }

    /// <summary>
    /// <para>Attach a component to an entity.</para>
    /// <para>给实体挂载组件。</para>
    /// </summary>
    /// <remarks>
    /// <para>如果实体已经拥有同类型组件，Arch 会抛出异常；需要覆盖时请用 <see cref="SetComponent{T}"/>。</para>
    /// <para>Adding a component the entity already has throws; use <see cref="SetComponent{T}"/> to replace it.</para>
    /// </remarks>
    public void AddComponent<T>(Entity entity, T component) where T : struct, IComponent
    {
        EnsureOwned(entity);
        _world.Add(entity.Handle, in component);
        IncreaseComponentCount(typeof(T));
    }

    /// <summary>
    /// <para>Add the component or replace its value if it already exists.</para>
    /// <para>添加组件；如果已存在则覆盖其值。</para>
    /// </summary>
    public void SetComponent<T>(Entity entity, T component) where T : struct, IComponent
    {
        EnsureOwned(entity);
        if (_world.Has<T>(entity.Handle))
        {
            _world.Set(entity.Handle, in component);
            return;
        }

        _world.Add(entity.Handle, in component);
        IncreaseComponentCount(typeof(T));
    }

    /// <summary>
    /// <para>Get a mutable reference to a component. The entity must own that component.</para>
    /// <para>获取组件的可变引用；实体必须已经挂载该组件。</para>
    /// </summary>
    /// <remarks>
    /// <para>The reference points into the component storage. Re-fetch it after any structural change (Add/Remove/Destroy).</para>
    /// <para>该引用指向组件存储内部；发生 Add/Remove/Destroy 等结构变更后必须重新获取。</para>
    /// </remarks>
    public ref T GetComponent<T>(Entity entity) where T : struct, IComponent
    {
        EnsureOwned(entity);
        return ref _world.Get<T>(entity.Handle);
    }

    /// <summary>
    /// <para>Try to copy a component from the entity.</para>
    /// <para>尝试读取组件；不存在时返回 false，不会抛异常。</para>
    /// </summary>
    public bool TryGetComponent<T>(Entity entity, out T component) where T : struct, IComponent
    {
        EnsureOwned(entity);
        return _world.TryGet(entity.Handle, out component);
    }

    /// <summary>
    /// <para>Remove a component from the entity. The entity must own that component.</para>
    /// <para>移除实体上的组件；实体必须已经挂载该组件。</para>
    /// </summary>
    public void RemoveComponent<T>(Entity entity) where T : struct, IComponent
    {
        EnsureOwned(entity);
        _world.Remove<T>(entity.Handle);
        DecreaseComponentCount(typeof(T));
    }

    /// <summary>
    /// <para>Destroy the entity together with all of its components. Destroying it twice is a no-op.</para>
    /// <para>销毁实体及其全部组件；重复销毁不会报错。</para>
    /// </summary>
    public void DestroyEntity(Entity entity)
    {
        ThrowIfDisposed();
        if (!IsAlive(entity))
        {
            return;
        }

        var signature = _world.GetSignature(entity.Handle);
        _world.Destroy(entity.Handle);

        foreach (var componentType in signature.Components)
        {
            DecreaseComponentCount(componentType.Type);
        }
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        _disposed = true;
        _componentCounts.Clear();
        _world.Dispose();
    }

    private void IncreaseComponentCount(Type componentType)
    {
        _componentCounts[componentType] = _componentCounts.GetValueOrDefault(componentType) + 1;
    }

    private void DecreaseComponentCount(Type componentType)
    {
        if (!_componentCounts.TryGetValue(componentType, out var count))
        {
            return;
        }

        if (count <= 1)
        {
            _componentCounts.Remove(componentType);
            return;
        }

        _componentCounts[componentType] = count - 1;
    }

    private void EnsureOwned(Entity entity)
    {
        ThrowIfDisposed();
        if (entity.Handle.WorldId != _world.Id || entity.Handle.Version <= 0)
        {
            throw new ArgumentException("The entity does not belong to this entity manager.", nameof(entity));
        }
    }

    private void ThrowIfDisposed()
    {
        ObjectDisposedException.ThrowIf(_disposed, this);
    }
}