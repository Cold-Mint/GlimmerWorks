using Microsoft.Xna.Framework;

namespace GlimmerWorks.Engine;

public static class ColorUtils
{
    /// <summary>
    /// <para>Blend one color channel (R/G/B) of two colors by opacity.</para>
    /// <para>按不透明度混合两个颜色的单个通道（R/G/B）。</para>
    /// </summary>
    /// <param name="b">Base color channel. 底色通道值（0–255）。</param>
    /// <param name="o">Overlay color channel. 覆盖色通道值（0–255）。</param>
    /// <param name="opacity">0 = base, 1 = overlay. 0 用底色，1 用覆盖色。</param>
    /// <returns>Mixed channel value. 混合后的通道值（0–255）。</returns>
    private static byte Blend(byte b, byte o, float opacity)
    {
        var inv = 1f - opacity;
        return (byte)Math.Round(b * inv + o * opacity);
    }

    /// <summary>
    /// <para>Put overlay color on top of base color with given opacity.</para>
    /// <para>把覆盖色按指定不透明度叠到底色上。</para>
    /// <para>Only RGB changes. Alpha stays the same as base color.</para>
    /// <para>只改 RGB，透明度（alpha）保持和底色一样。</para>
    /// </summary>
    /// <param name="baseColor">The background color. 底色（背景色）。</param>
    /// <param name="overlay">The color on top. 覆盖色（上层颜色）。</param>
    /// <param name="opacity">0 = only base, 1 = only overlay. 0 只有底色，1 只有覆盖色。</param>
    /// <returns>The final mixed color. 最终混合出来的颜色。</returns>
    public static Color Overlay(Color baseColor, Color overlay, float opacity)
    {
        opacity = Math.Clamp(opacity, 0F, 1F);
        return new Color(
            Blend(baseColor.R, overlay.R, opacity),
            Blend(baseColor.G, overlay.G, opacity),
            Blend(baseColor.B, overlay.B, opacity),
            baseColor.A);
    }
}