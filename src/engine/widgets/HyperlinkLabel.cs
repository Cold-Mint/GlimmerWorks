using System.Diagnostics;
using GlimmerWorks.Game;
using Myra.Events;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.Widgets;

public sealed class HyperlinkLabel : Myra.Graphics2D.UI.Label
{
    private readonly Link? _link;

    public Link? Link
    {
        get => _link;
        init
        {
            _link = value;
            Text = value?.Title ?? string.Empty;
        }
    }

    public HyperlinkLabel(Theme theme, Link? link = null)
    {
        _link = link;
        Text = link?.Title ?? string.Empty;
        TextColor = theme.Primary;
        Font = theme.GetFont(ThemeFontKind.Normal);
        OverTextColor = theme.PrimaryOver();
        MinHeight = theme.MinHeight;
        MouseCursor = MouseCursorType.Hand;
        TouchDown += OnTouchDown;
    }

    private void OnTouchDown(object sender, MyraEventArgs e)
    {
        if (string.IsNullOrWhiteSpace(Link?.Url))
        {
            return;
        }

        if (!Uri.TryCreate(Link.Url, UriKind.Absolute, out var uri))
        {
            return;
        }

        if (uri.Scheme != Uri.UriSchemeHttp && uri.Scheme != Uri.UriSchemeHttps)
        {
            return;
        }

        Process.Start(new ProcessStartInfo
        {
            FileName = uri.AbsoluteUri,
            UseShellExecute = true
        });
    }
}