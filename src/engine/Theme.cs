using FontStashSharp;
using Microsoft.Xna.Framework;
using Myra.Graphics2D;

namespace GlimmerWorks.Engine;

public enum ThemeFontKind
{
    Small,
    Normal,
    Large,
    ExtraLarge
}

public class Theme
{
    public int Radius { get; set; } = 18;
    public int CheckBoxRadius { get; set; } = 4;

    public int MinHeight { get; set; } = 40;
    public int InputTextMinWidth { get; set; } = 512;
    public int ButtonMinWidth { get; set; } = 240;

    public Thickness ButtonPadding { get; set; } = new(20, 8);
    public Thickness InputTextPadding { get; set; } = new(16, 16);


    /// <summary>
    /// <para>Primary</para>
    /// <para>主色调</para>
    /// </summary>
    public Color Primary { get; set; } = new(0x67, 0x50, 0xA4);

    public int BorderWidth { get; set; } = 2;

    /// <summary>
    /// <para>Surface</para>
    /// <para>表面颜色</para>
    /// </summary>
    public Color Surface = new(242, 242, 247, 255);

    /// <summary>
    /// <para>OnPrimary</para>
    /// <para>主色调上的内容色（文字、图标），与 Primary 搭配使用以保证对比度</para>
    /// </summary>
    public Color OnPrimary { get; set; } = new(0xFF, 0xFF, 0xFF);

    public Color InputTextBackground { get; set; } = new(0xE6, 0xE0, 0xE9);

    public Color PrimaryOver()
    {
        return ColorUtils.Overlay(Primary, OnPrimary, 0.16F);
    }

    public Color PrimaryPressed()
    {
        return ColorUtils.Overlay(Primary, OnPrimary, 0.25F);
    }

    /// <summary>
    /// <para>OnSurface</para>
    /// <para>按钮</para>
    /// </summary>
    public Color OnSurface { get; set; } = new(0x1C, 0x1B, 0x1F);


    public Color Info { get; set; } = Color.White;
    public Color Warn { get; set; } = Color.Yellow;
    public Color Error { get; set; } = Color.Red;

    private readonly FontSystem _fontSystem = new();

    public SpriteFontBase GetFont(ThemeFontKind kind)
    {
        return kind switch
        {
            ThemeFontKind.Small => _fontSystem.GetFont(48),
            ThemeFontKind.Normal => _fontSystem.GetFont(64),
            ThemeFontKind.Large => _fontSystem.GetFont(96),
            ThemeFontKind.ExtraLarge => _fontSystem.GetFont(128),
            _ => _fontSystem.GetFont(48)
        };
    }


    public void Reset()
    {
        _fontSystem.Reset();
    }

    public void AddFont(Stream stream)
    {
        using (stream)
        {
            _fontSystem.AddFont(stream);
        }
    }
}