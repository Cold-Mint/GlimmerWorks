using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Engine.Ecs.Components;

/// <summary>
/// <para>ItemDropComponent</para>
/// <para>地面上的一堆掉落物：物品引用与数量。</para>
/// </summary>
public struct ItemDropComponent : IComponent
{
    public ResourceRef Item;
    public int Count;
}
