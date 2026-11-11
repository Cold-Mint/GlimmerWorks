using FontStashSharp.RichText;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class Label : Myra.Graphics2D.UI.Label
{
    public Label(Theme theme, string text)
    {
        Text = text;
        TextColor = theme.OnSurface;
        MinHeight = theme.MinHeight;
        Font = theme.GetFont(ThemeFontKind.Normal);
        TextAlign = TextHorizontalAlignment.Center;
        VerticalAlignment = VerticalAlignment.Center;
    }
}