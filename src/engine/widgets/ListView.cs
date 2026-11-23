using Myra.Graphics2D.Brushes;

namespace GlimmerWorks.Engine.Widgets;

public class ListView : Myra.Graphics2D.UI.ListView
{
    public ListView(Theme theme)
    {
        Background = new SolidBrush(theme.Surface);
    }
}