namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>A decoded texture: 8-bit RGBA pixels.</para>
/// <para>解码后的纹理：8 位 RGBA 像素。</para>
/// </summary>
public sealed class TextureData
{
    /// <summary>
    /// <para>Width in pixels.</para>
    /// <para>宽度（像素）</para>
    /// </summary>
    public int Width { get; set; }

    /// <summary>
    /// <para>Height in pixels.</para>
    /// <para>高度（像素）</para>
    /// </summary>
    public int Height { get; set; }

    /// <summary>
    /// <para>RGBA8 pixels, row-major, top to bottom.</para>
    /// <para>RGBA8 像素，按行排列，从上到下。</para>
    /// </summary>
    public byte[] Pixels { get; set; } = [];

    /// <summary>
    /// <para>The source file name, used for diagnostics.</para>
    /// <para>源文件名，用于诊断。</para>
    /// </summary>
    public string? Name { get; set; }
}
