using System.Reflection;
using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.Engine.widgets;
using Microsoft.Xna.Framework;
using Myra.Events;
using Myra.Graphics2D;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;
using Label = GlimmerWorks.Engine.widgets.Label;

namespace GlimmerWorks.Game;

public class MainScene(AppContext appContext) : Scene(appContext)
{
    private readonly List<Link> _links =
    [
        new() { Text = "Github", Url = "https://github.com/Cold-Mint/GlimmerWorks" },
        new() { Text = "Discord", Url = "https://discord.com/invite/CfppC9WHw8" },
        new() { Text = "itch.io", Url = "https://cold-mint.itch.io/glimmerworks" },
        new() { Text = "QQ Channel", Url = "https://pd.qq.com/s/cntb09fr1?b=9" }
    ];

    public override void OnCreate()
    {
        var panel = new Panel
        {
            HorizontalAlignment = HorizontalAlignment.Stretch,
            VerticalAlignment = VerticalAlignment.Stretch
        };
        var buttonStack = new VerticalStackPanel
        {
            Spacing = 16,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Center,
            Top = 80
        };

        var startButton = new FilledButton(AppContext.Theme, AppContext.LocalizationManager.Get("ui.start_game"));
        startButton.Click += StartButtonOnClick;
        buttonStack.Widgets.Add(startButton);
        var exitButton = new OutlinedButton(AppContext.Theme, AppContext.LocalizationManager.Get("ui.exit_game"));
        exitButton.Click += ExitButtonOnClick;
        buttonStack.Widgets.Add(exitButton);

        panel.Widgets.Add(buttonStack);

        var copyrightLabel = new Label(AppContext.Theme, GetCopyright())
        {
            Font = AppContext.Theme.GetFont(ThemeFontKind.Small),
            HorizontalAlignment = HorizontalAlignment.Left,
            VerticalAlignment = VerticalAlignment.Bottom,
            Margin = new Thickness(16, 0, 0, 16)
        };
        panel.Widgets.Add(copyrightLabel);

        var versionLabel = new Label(AppContext.Theme, GetGameVersion())
        {
            Font = AppContext.Theme.GetFont(ThemeFontKind.Small),
            HorizontalAlignment = HorizontalAlignment.Right,
            VerticalAlignment = VerticalAlignment.Top,
            Margin = new Thickness(0, 16, 16, 0)
        };
        panel.Widgets.Add(versionLabel);

        var linksStack = new HorizontalStackPanel
        {
            Spacing = 20,
            HorizontalAlignment = HorizontalAlignment.Right,
            VerticalAlignment = VerticalAlignment.Bottom,
            Margin = new Thickness(0, 0, 16, 16)
        };

        foreach (var link in _links)
        {
            var linkLabel = new HyperlinkLabel(theme: AppContext.Theme)
                { Link = link, Font = AppContext.Theme.GetFont(ThemeFontKind.Small) };
            linksStack.Widgets.Add(linkLabel);
        }

        panel.Widgets.Add(linksStack);
        SetWidget(panel);
    }

    private string GetCopyright()
    {
        const int copyrightStartYear = 2025;
        var currentYear = Math.Max(DateTime.Now.Year, copyrightStartYear);
        return currentYear == copyrightStartYear
            ? $"Copyright (C) {copyrightStartYear} Cold-Mint"
            : $"Copyright (C) {copyrightStartYear}-{currentYear} Cold-Mint";
    }

    private string GetGameVersion()
    {
        var assembly = Assembly.GetExecutingAssembly();
        var info = assembly
            .GetCustomAttribute<AssemblyInformationalVersionAttribute>()
            ?.InformationalVersion;

        if (!string.IsNullOrWhiteSpace(info))
        {
            var plusIndex = info.IndexOf('+');
            if (plusIndex >= 0)
                info = info[..plusIndex];

            return $"v{info}";
        }

        var ver = assembly.GetName().Version;
        return ver != null ? $"v{ver.Major}.{ver.Minor}.{ver.Build}" : "v0.0.0";
    }

    private void ExitButtonOnClick(object sender, MyraEventArgs e)
    {
        AppContext.SceneStack.AddTask(
            new SceneStackTask { Type = StackType.Pop });
    }

    private void StartButtonOnClick(object sender, MyraEventArgs e)
    {
        AppContext.SceneStack.AddTask(
            new SceneStackTask { Type = StackType.Push, Scene = new CreateWorldScene(AppContext) });
    }
}