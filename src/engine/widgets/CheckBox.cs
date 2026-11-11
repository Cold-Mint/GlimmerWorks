using Microsoft.Xna.Framework;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.widgets;

public sealed class CheckBox : CheckButton
{
    public CheckBox(Theme theme)
    {
        var size = theme.MinHeight;
        CheckImage.Width = size;
        CheckImage.Height = size;
        var transparent = new SolidBrush(Color.Transparent);
        CheckImage.Background = transparent;
        CheckImage.OverBackground = transparent;
        CheckImage.PressedBackground = transparent;
        CheckImage.DisabledBackground = transparent;
        var hollow = new RoundedCornersHollowBrush(
                theme.Primary, Color.Transparent, theme.BorderWidth)
            { Radius = theme.CheckBoxRadius };

        var hollowHover = new RoundedCornersHollowBrush(
                theme.Primary, new Color(theme.Primary, 0.08f), theme.BorderWidth)
            { Radius = theme.CheckBoxRadius };

        var hollowPressed = new RoundedCornersHollowBrush(
                theme.Primary, new Color(theme.Primary, 0.12f), theme.BorderWidth)
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