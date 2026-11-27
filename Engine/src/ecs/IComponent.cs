namespace GlimmerWorks.Engine.Ecs;

/// <summary>
/// <para>IComponent</para>
/// <para>标记接口：可存放在 Arch 实体上的组件数据。</para>
/// </summary>
/// <remarks>
/// <para>组件优先使用 struct，避免每个实体产生堆分配。组件应为纯数据，不包含行为。</para>
/// <para>Composition over inheritance: entities are just ids, all state lives in components.</para>
/// </remarks>
public interface IComponent
{
}
