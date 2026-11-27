using GlimmerWorks.Game.Ui.Theme;
using Myra.Graphics2D.Brushes;

namespace GlimmerWorks.Game.Widgets;

public sealed class ListView : Myra.Graphics2D.UI.ListView
{
    public ListView(GameTheme theme)
    {
        Background = new SolidBrush(theme.Surface);
    }
}