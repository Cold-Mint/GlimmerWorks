using FontStashSharp.RichText;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game.Widgets;

public sealed class OutlinedButton : Button
{
    public OutlinedButton(Theme theme, string text)
    {
        Background = new RoundedCornersHollowBrush(theme.Primary, theme.Transparent, theme.BorderWidth)
            { Radius = theme.Radius };
        OverBackground = new RoundedCornersHollowBrush(theme.PrimaryOver(), theme.Transparent, theme.BorderWidth)
            { Radius = theme.Radius };
        PressedBackground = new RoundedCornersHollowBrush(theme.PrimaryPressed(), theme.Transparent, theme.BorderWidth)
            { Radius = theme.Radius };
        MouseCursor = MouseCursorType.Hand;
        Content = new Myra.Graphics2D.UI.Label
        {
            Text = text,
            TextColor = theme.Primary,
            Padding = theme.ButtonPadding,
            Font = theme.GetFont(ThemeFontKind.Normal),
            TextAlign = TextHorizontalAlignment.Center,
            MinHeight = theme.MinHeight,
            MinWidth = theme.ButtonMinWidth,
            VerticalAlignment = VerticalAlignment.Center,
        };
    }
}