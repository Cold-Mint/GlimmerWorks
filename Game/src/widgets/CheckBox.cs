using FontStashSharp;
using GlimmerWorks.Game.Ui.Theme;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game.Widgets;

public sealed class CheckBox : CheckButton
{
    public CheckBox(GameTheme theme)
    {
        var size = theme.MinHeight;
        CheckImage.Width = size;
        CheckImage.Height = size;
        var transparent = new SolidBrush(GameTheme.Transparent);
        CheckImage.Background = transparent;
        CheckImage.OverBackground = transparent;
        CheckImage.PressedBackground = transparent;
        CheckImage.DisabledBackground = transparent;
        var hollow = new RoundedCornersHollowBrush(
                theme.Primary, GameTheme.Transparent, theme.BorderWidth)
            { Radius = theme.CheckBoxRadius };

        var hollowHover = new RoundedCornersHollowBrush(
                theme.Primary, new FSColor(theme.Primary, 0.08F), theme.BorderWidth)
            { Radius = theme.CheckBoxRadius };

        var hollowPressed = new RoundedCornersHollowBrush(
                theme.Primary, new FSColor(theme.Primary, 0.12F), theme.BorderWidth)
            { Radius = theme.CheckBoxRadius };

        var solidPrimary = new RoundedCornersSolidBrush(theme.Primary)
            { Radius = theme.CheckBoxRadius };

        Background = hollow;
        OverBackground = hollowHover;
        PressedBackground = hollowPressed;
        IsCheckedChanged += (_, _) =>
        {
            if (IsChecked)
            {
                Background = solidPrimary;
                OverBackground = solidPrimary;
                PressedBackground = solidPrimary;
            }
            else
            {
                Background = hollow;
                OverBackground = hollowHover;
                PressedBackground = hollowPressed;
            }
        };
        MouseCursor = MouseCursorType.Hand;
        CheckContentSpacing = 8;
    }
}