using FontStashSharp.RichText;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class Label : Myra.Graphics2D.UI.Label
{
    public Label(Theme theme)
    {
        TextColor = theme.OnSurface;
        MinHeight = theme.MinHeight;
        Font = theme.GetFont(ThemeFontKind.Normal);
        TextAlign = TextHorizontalAlignment.Center;
        VerticalAlignment = VerticalAlignment.Center;
    }
}