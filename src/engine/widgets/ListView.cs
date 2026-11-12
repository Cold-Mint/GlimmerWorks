using Myra.Graphics2D.Brushes;

namespace GlimmerWorks.Engine.widgets;

public class ListView : Myra.Graphics2D.UI.ListView
{
    public ListView(Theme theme)
    {
        Background = new SolidBrush(theme.Surface);
    }
}