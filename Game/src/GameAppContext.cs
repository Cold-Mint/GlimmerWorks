using System.Globalization;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Mods;
using GlimmerWorks.Game.Scene;
using AppContext = GlimmerWorks.Engine.AppContext;
using Microsoft.Xna.Framework;
using Myra.Graphics2D.UI;
using Microsoft.Extensions.Localization;

namespace GlimmerWorks.Game;

public class GameAppContext : AppContext
{
    public SceneStack SceneStack { get; init; } = new();

    public GameConfig GameConfig { get; } = new();

    public AssetsPackManager AssetsPackManager { get; init; } = new();

    public DataPackManager DataPackManager { get; init; } = new();

    public GraphicsDeviceManager? GraphicsDeviceManager { get; set; }
    public LocalizationManager LocalizationManager { get; init; } = new();
    public IStringLocalizer Localizer => LocalizationManager;

    public Theme Theme { get; init; } = new();

    public Desktop? Desktop { get; set; }

    public GameAppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
        // Cover the game configuration.
        // 覆盖游戏配置。
        var gameConfig =
            VirtualFileSystem.ReadToml<GameConfig>(new VirtualPath("assets", "gameConfig.toml"));
        if (gameConfig != null)
        {
            GameConfig = gameConfig;
        }

        // Use the configured language if set, otherwise follow the system UI language.
        // 配置了语言则使用配置语言，否则跟随系统 UI 语言。
        ChangeLanguage(string.IsNullOrWhiteSpace(GameConfig.Language)
            ? CultureInfo.CurrentUICulture.Name
            : GameConfig.Language);
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
        LocalizationManager.CurrentLanguage = language;
        var candidates = LocalizationManager.GetCandidates(language);
        bool loadFont = false;
        bool loadLangs = false;
        foreach (var candidate in candidates)
        {
            var poFile = new VirtualPath("assets", "langs", $"{candidate}.po");
            if (!loadLangs && VirtualFileSystem.FileExists(poFile) &&
                LocalizationManager.LoadPoFile(VirtualFileSystem, poFile))
            {
                loadLangs = true;
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