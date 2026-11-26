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
        Width = data.Width;
        Height = data.Height;
    }

    /// <summary>
    /// <para>Width in pixels.</para>
    /// <para>宽度（像素）。</para>
    /// </summary>
    public int Width { get; }

    /// <summary>
    /// <para>Height in pixels.</para>
    /// <para>高度（像素）。</para>
    /// </summary>
    public int Height { get; }

    internal TextureData Data { get; }

    internal nint Handle { get; set; }
}
