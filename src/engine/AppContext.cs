using System.Globalization;
using FontStashSharp;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.engine.Vfs;
using GlimmerWorks.Game;

namespace GlimmerWorks.Engine;

public class AppContext
{
    public LogCat LogCat { get; init; } = new();
    public LocalizationManager LocalizationManager { get; init; } = new();
    public GameConfig GameConfig { get; } = new();
    public VirtualFileSystem VirtualFileSystem { get; } = new();

    public DataRepository DataRepository { get; init; } = new();

    public SceneStack SceneStack { get; init; } = new();

    public FontSystem FontSystem { get; init; } = new();

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
        LogCat.Localization = LocalizationManager;
    }

    /// <summary>
    /// <para>ChangeLanguage</para>
    /// <para>改变语言</para>
    /// </summary>
    /// <param name="language"></param>
    void ChangeLanguage(string language)
    {
        FontSystem.Reset();
        LocalizationManager.Clear();
        var candidates = LocalizationManager.GetCandidates(language);
        bool loadFont = false;
        bool loadLangs = false;
        foreach (var candidate in candidates)
        {
            var langsRoot = new VirtualPath("assets", "langs", candidate);
            if (!loadLangs && VirtualFileSystem.DirectoryExists(langsRoot))
            {
                if (LocalizationManager.LoadDirectory(LogCat, VirtualFileSystem, langsRoot))
                {
                    loadLangs = true;
                }
            }

            var ttfVirtualPath = new VirtualPath("assets", "fonts", $"{candidate}.ttf");
            if (!loadFont && VirtualFileSystem.FileExists(ttfVirtualPath))
            {
                using var stream = VirtualFileSystem.OpenRead(ttfVirtualPath);
                if (stream == null)
                {
                    continue;
                }

                FontSystem.AddFont(stream);
                loadFont = true;
            }

            if (loadFont && loadLangs)
            {
                break;
            }
        }
    }
}