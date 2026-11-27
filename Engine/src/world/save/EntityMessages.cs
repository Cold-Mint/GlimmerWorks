using MemoryPack;

namespace GlimmerWorks.Engine.World.Save;

/// <summary>
/// <para>ItemDropMessage</para>
/// <para><c>ItemDropComponent</c> 的存档载荷。物品引用本身放在 <see cref="EntityItemMessage.ResourceRef"/>。</para>
/// </summary>
[MemoryPackable]
public sealed partial class ItemDropMessage
{
    public int Count { get; set; } = 1;

    public Vector2DMessage Cell { get; set; } = new();
}
