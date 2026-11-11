using FontStashSharp.RichText;
using Myra.Events;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class TextButton : Myra.Graphics2D.UI.Label
{
    public event MyraEventHandler? Click;

    public TextButton(Theme theme, string text)
    {
        TextColor = theme.Primary;
        Padding = theme.ButtonPadding;
        Text = text;
        Font = theme.GetFont(ThemeFontKind.Normal);
        OverTextColor = theme.PrimaryOver();
        MinHeight = theme.MinHeight;
        MinWidth = theme.ButtonMinWidth;
        TouchDown += OnTouchDown;
        TextAlign = TextHorizontalAlignment.Center;
        MouseCursor = MouseCursorType.Hand;
        VerticalAlignment = VerticalAlignment.Center;
    }

    private void OnTouchDown(object sender, MyraEventArgs e)
    {
        Click?.Invoke(sender, e);
    }
}