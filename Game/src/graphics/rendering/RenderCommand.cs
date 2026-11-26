using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Game.Graphics.Rendering;

/// <summary>
/// <para>One draw operation: either a batch of UI primitives or one material quad batch.</para>
/// <para>一次绘制操作：一组 UI 图元，或一批材质四边形。</para>
/// </summary>
internal sealed class RenderCommand
{
    public bool IsMaterial { get; init; }

    public Texture2D? Texture { get; set; }

    public Material? Material { get; init; }

    public nint Pipeline { get; set; }

    public Color? Tint { get; init; }

    public int FirstVertex { get; init; }

    public int VertexCount { get; set; }
}
