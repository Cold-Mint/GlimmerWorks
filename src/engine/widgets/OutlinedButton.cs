using FontStashSharp.RichText;
using Microsoft.Xna.Framework;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class OutlinedButton : Button
{
    public OutlinedButton(Theme theme, string text)
    {
        Background = new RoundedCornersHollowBrush(theme.Primary, Color.Transparent, theme.BorderWidth)
            { Radius = theme.Radius };
        OverBackground = new RoundedCornersHollowBrush(theme.PrimaryOver(), Color.Transparent, theme.BorderWidth)
            { Radius = theme.Radius };
        PressedBackground = new RoundedCornersHollowBrush(theme.PrimaryPressed(), Color.Transparent, theme.BorderWidth)
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