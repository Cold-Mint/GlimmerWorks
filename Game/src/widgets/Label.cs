using FontStashSharp.RichText;
using GlimmerWorks.Game.Ui.Theme;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game.Widgets;

public sealed class Label : Myra.Graphics2D.UI.Label
{
    public Label(GameTheme theme)
    {
        TextColor = theme.OnSurface;
        MinHeight = theme.MinHeight;
        Font = theme.GetFont(ThemeFontKind.Normal);
        TextAlign = TextHorizontalAlignment.Center;
        VerticalAlignment = VerticalAlignment.Center;
    }
}