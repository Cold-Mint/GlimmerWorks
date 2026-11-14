using System.Globalization;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;
using Microsoft.Xna.Framework;
using Myra;
using Myra.Graphics2D;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;
using Microsoft.Extensions.Localization;

namespace GlimmerWorks.Engine;

public class AppContext
{
    public GraphicsDeviceManager? GraphicsDeviceManager { get; set; }
    public LocalizationManager LocalizationManager { get; init; } = new();
    public IStringLocalizer Localizer => LocalizationManager;
    public GameConfig GameConfig { get; } = new();
    public VirtualFileSystem VirtualFileSystem { get; } = new();

    public DataRepository DataRepository { get; init; } = new();

    public SceneStack SceneStack { get; init; } = new();

    public Theme Theme { get; init; } = new();

    public Desktop? Desktop
    {
        get;
        set
        {
            field = value;
            ChangeStylesheet();
        }
    }

    public AppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
        // Cover the game configuration.
        // 覆盖游戏配置。
        var gameConfig = VirtualFileSystem.ReadToml<GameConfig>(new VirtualPath("assets", "config.toml"));
        if (gameConfig != null)
        {
            GameConfig = gameConfig;
        }

        // Use the configured language if set, otherwise follow the system UI language.
        // 配置了语言则使用配置语言，否则跟随系统 UI 语言。
        ChangeLanguage(string.IsNullOrEmpty(GameConfig.Language)
            ? CultureInfo.CurrentUICulture.Name
            : GameConfig.Language);
    }

    /// <summary>
    /// <para>ChangeStylesheet</para>
    /// <para>改变样式</para>
    /// </summary>
    private void ChangeStylesheet()
    {
        MyraEnvironment.TooltipDelayInMs = 15;
        MyraEnvironment.TooltipOffset = new Point(12, 10);
        MyraEnvironment.TooltipCreator = widget =>
        {
            if (string.IsNullOrEmpty(widget.Tooltip))
            {
                return null;
            }

            return new Panel
            {
                Background = new RoundedCornersSolidBrush(new Color(28, 28, 30, 245)) { Radius = Theme.Radius },
                Padding = new Thickness(12, 8),
                Widgets =
                {
                    new Label
                    {
                        Text = widget.Tooltip,
                        TextColor = Theme.OnPrimary,
                        Font = Theme.GetFont(ThemeFontKind.Small)
                    }
                }
            };
        };
    }

    /// <summary>
    /// <para>ChangeLanguage</para>
    /// <para>改变语言</para>
    /// </summary>
    /// <param name="language"></param>
    private void ChangeLanguage(string language)
    {
        Theme.Reset();
        LocalizationManager.Clear();
        var candidates = LocalizationManager.GetCandidates(language);
        bool loadFont = false;
        bool loadLangs = false;
        foreach (var candidate in candidates)
        {
            var poFile = new VirtualPath("assets", "langs", $"{candidate}.po");
            if (!loadLangs && VirtualFileSystem.FileExists(poFile))
            {
                if (LocalizationManager.LoadPoFile(VirtualFileSystem, poFile))
                {
                    loadLangs = true;
                }
            }

            var ttfVirtualPath = new VirtualPath("assets", "fonts", $"{candidate}.ttf");
            if (!loadFont && VirtualFileSystem.FileExists(ttfVirtualPath))
            {
                var stream = VirtualFileSystem.OpenRead(ttfVirtualPath);
                if (stream == null)
                {
                    continue;
                }

                Theme.AddFont(stream);
                loadFont = true;
            }

            if (loadFont && loadLangs)
            {
                break;
            }
        }
    }
}