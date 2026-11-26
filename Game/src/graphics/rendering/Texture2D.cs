namespace GlimmerWorks.Game.Graphics.Rendering;

/// <summary>
/// <para>A texture managed by <see cref="Renderer2D"/>.</para>
/// <para>由 <see cref="Renderer2D"/> 管理的纹理。</para>
/// </summary>
internal sealed class Texture2D
{
    internal Texture2D(TextureData data)
    {
        Data = data;
    }

    internal TextureData Data { get; }

    internal nint Handle { get; set; }
}
