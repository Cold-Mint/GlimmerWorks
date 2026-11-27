using GlimmerWorks.Game.Ui.Theme;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game.Widgets;

public sealed class InputText : TextBox
{
    public InputText(GameTheme theme)
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