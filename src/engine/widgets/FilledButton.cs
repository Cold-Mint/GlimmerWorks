using FontStashSharp.RichText;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class FilledButton : Button
{
    public FilledButton(Theme theme, string text)
    {
        Background = new RoundedCornersSolidBrush(theme.Primary) { Radius = theme.Radius };
        OverBackground = new RoundedCornersSolidBrush(theme.PrimaryOver()) { Radius = theme.Radius };
        PressedBackground = new RoundedCornersSolidBrush(theme.PrimaryPressed()) { Radius = theme.Radius };
        MouseCursor = MouseCursorType.Hand;
        Content = new Myra.Graphics2D.UI.Label
        {
            Text = text,
            TextColor = theme.OnPrimary,
            Padding = theme.ButtonPadding,
            Font = theme.GetFont(ThemeFontKind.Normal),
            TextAlign = TextHorizontalAlignment.Center,
            MinHeight = theme.MinHeight,
            MinWidth = theme.ButtonMinWidth,
            VerticalAlignment = VerticalAlignment.Center,
        };
    }
}