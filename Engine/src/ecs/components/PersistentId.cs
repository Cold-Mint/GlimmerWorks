namespace GlimmerWorks.Engine.Ecs.Components;

/// <summary>
/// <para>PersistentId</para>
/// <para>存档稳定的实体编号。运行时实体句柄（Entity）每次加载都会变，只有这个值跨存档保持。</para>
/// </summary>
public struct PersistentId : IComponent
{
    public ulong Value;
}
