using FontStashSharp;
using Myra.Graphics2D;

namespace GlimmerWorks.Game;

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
    public FSColor Primary { get; set; } = new(103, 80, 164);

    public static FSColor Transparent => new(0, 0, 0, 0);

    public int BorderWidth { get; set; } = 2;

    /// <summary>
    /// <para>Surface</para>
    /// <para>表面颜色</para>
    /// </summary>
    public FSColor Surface { get; set; } = new(242, 242, 247);

    /// <summary>
    /// <para>OnPrimary</para>
    /// <para>主色调上的内容色（文字、图标），与 Primary 搭配使用以保证对比度</para>
    /// </summary>
    public FSColor OnPrimary { get; set; } = new(255, 255, 255);

    public FSColor InputTextBackground { get; set; } = new(230, 224, 233);

    public FSColor PrimaryOver()
    {
        return ColorUtils.Overlay(Primary, OnPrimary, 0.16F);
    }

    public FSColor PrimaryPressed()
    {
        return ColorUtils.Overlay(Primary, OnPrimary, 0.25F);
    }

    /// <summary>
    /// <para>OnSurface</para>
    /// <para>按钮</para>
    /// </summary>
    public FSColor OnSurface { get; set; } = new(28, 27, 31);


    public FSColor Info { get; set; } = new(255, 255, 255);
    public FSColor Warn { get; set; } = new(255, 255, 0);
    public FSColor Error { get; set; } = new(255, 0, 0);

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

    public void AddFont(Stream stream)
    {
        using (stream)
        {
            _fontSystem.AddFont(stream);
        }
    }
}