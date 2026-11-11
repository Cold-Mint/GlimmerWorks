using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class InputText : TextBox
{
    public InputText(Theme theme)
    {
        MouseCursor = MouseCursorType.IBeam;
        Font = theme.GetFont(ThemeFontKind.Normal);
        MinHeight = theme.MinHeight;
        MinWidth = theme.InputTextMinWidth;
        TextColor = theme.OnSurface;
        Padding = theme.InputTextPadding;
        Background = new RoundedCornersSolidBrush(theme.InputTextBackground) { Radius = theme.Radius };
    }
}