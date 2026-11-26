using System.Globalization;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Graphics.Uniform;
using GlimmerWorks.Game.Mods;
using GlimmerWorks.Game.Scene;
using AppContext = GlimmerWorks.Engine.AppContext;
using Myra.Graphics2D.UI;
using Microsoft.Extensions.Localization;

namespace GlimmerWorks.Game;

public class GameAppContext : AppContext
{
    public SceneStack SceneStack { get; } = new();

    public GameConfig GameConfig { get; } = new();

    public AssetsPackManager AssetsPackManager { get; } = new();

    public RuntimeUniformManager RuntimeUniformManager { get; } = new();

    public DataPackManager DataPackManager { get; } = new();

    public LocalizationManager LocalizationManager { get; } = new();
    public IStringLocalizer Localizer => LocalizationManager;
    public Theme Theme { get; } = new();

    public Desktop? Desktop { get; set; }

    public GameAppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
        // Cover the game configuration.
        // 覆盖游戏配置。
        var gameConfig =
            VirtualFileSystem.ReadToml<GameConfig>(new VirtualPath("assets", "config.toml"));
        if (gameConfig != null)
        {
            GameConfig = gameConfig;
        }

        EngineConfig = GameConfig;
        LogCat.Debug("Update LogLevelMask.");
        LogCat.LogLevelMask = GameConfig.LogLevelMask;
        if (!CacheManager.ReadCacheManifest(this))
        {
            LogCat.Debug("Read cache manifest fail.");
        }

        AssetsPackManager.Order = GameConfig.AssetsPackOrder;
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

                loadFont = true;
            }

            if (loadFont && loadLangs)
            {
                break;
            }
        }
    }
}