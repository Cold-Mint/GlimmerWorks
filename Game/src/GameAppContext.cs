using System.Globalization;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Pipeline;
using GlimmerWorks.Game.Graphics.Uniform;
using GlimmerWorks.Game.Graphics.Vertex;
using GlimmerWorks.Game.Mods;
using GlimmerWorks.Game.Scene;
using GlimmerWorks.Game.Ui;
using GlimmerWorks.Game.Ui.Theme;
using AppContext = GlimmerWorks.Engine.AppContext;
using Microsoft.Extensions.Localization;
using SDL3;

namespace GlimmerWorks.Game;

public class GameAppContext : AppContext
{
    private const string AssetsDirectory = "assets";

    public SceneStack SceneStack { get; } = new();

    public GameConfig GameConfig { get; } = new();

    public AssetsPackManager AssetsPackManager { get; } = new();

    public RuntimeUniformManager RuntimeUniformManager { get; } = new();

    public DataPackManager DataPackManager { get; } = new();

    public LocalizationManager LocalizationManager { get; } = new();
    public IStringLocalizer Localizer => LocalizationManager;
    public GameTheme Theme { get; } = new();


    /// <summary>
    /// <para>The Myra UI manager, created once during startup.</para>
    /// <para>Myra UI 管理器，在启动时创建一次。</para>
    /// </summary>
    internal MyraGameUi? Ui { get; private set; }

    /// <summary>
    /// <para>InitializeUi</para>
    /// <para>使用给定的设备和窗口信息初始化 Myra UI。该操作幂等，仅在启动时调用一次。</para>
    /// </summary>
    /// <returns>UI 创建成功或已存在时返回 <c>true</c>；初始化失败时返回 <c>false</c>。</returns>
    internal bool InitializeUi(IntPtr device, IntPtr window, SDL.GPUTextureFormat swapchainFormat)
    {
        if (Ui != null)
        {
            return true;
        }

        if (device == IntPtr.Zero || window == IntPtr.Zero)
        {
            return false;
        }

        CompiledShader? vertexShader = null;
        CompiledShader? fragmentShader = null;

        if (AssetsPackManager.PackCount > 0)
        {
            var vertexResult = AssetsPackManager.FindAsset<CompiledShader>(this,
                ResourceRef.Create(ResourceType.VertShader, ResourceRef.CorePackId, "ui"));
            var fragmentResult = AssetsPackManager.FindAsset<CompiledShader>(this,
                ResourceRef.Create(ResourceType.FragShader, ResourceRef.CorePackId, "ui"));
            vertexShader = vertexResult.Exist ? vertexResult.Result : null;
            fragmentShader = fragmentResult.Exist ? fragmentResult.Result : null;
        }

        // The splash screen needs UI before asset packs are loaded.
        // 启动画面需要在资产包加载前使用 UI。
        vertexShader ??= ShaderCompiler.Compile(true, "builtin_ui.vert",
            System.Text.Encoding.UTF8.GetBytes(BuiltInUiShaders.Vertex));
        fragmentShader ??= ShaderCompiler.Compile(false, "builtin_ui.frag",
            System.Text.Encoding.UTF8.GetBytes(BuiltInUiShaders.Fragment));

        if (vertexShader == null || fragmentShader == null)
        {
            LogCat.Error("Failed to create the Myra UI shaders.");
            return false;
        }

        var pipeline = new GraphicsPipeline
        {
            BlendMode = BlendMode.Alpha,
            VertexShader = vertexShader,
            FragmentShader = fragmentShader,
            VertexLayout = VertexLayout.PositionTextureColor
        };
        Ui = new MyraGameUi(device, window, swapchainFormat, pipeline);
        LogCat.Info("Myra UI initialized.");
        return true;
    }

    public GameAppContext()
    {
        VirtualFileSystem.AddFileSystemProvider(new LocalFileSystemProvider(System.AppContext.BaseDirectory));
        // Cover the game configuration.
        // 覆盖游戏配置。
        var gameConfig =
            VirtualFileSystem.ReadToml<GameConfig>(new VirtualPath(AssetsDirectory, "config.toml"));
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

        var loadFont = false;
        var loadLangs = false;
        foreach (var candidate in LocalizationManager.GetCandidates(language))
        {
            loadLangs = loadLangs || TryLoadLanguage(candidate);
            loadFont = loadFont || TryLoadFont(candidate);
            if (loadFont && loadLangs)
            {
                break;
            }
        }

        if (!loadFont)
        {
            LoadDefaultFont();
        }
    }

    /// <summary>
    /// <para>Try to load the .po file for a language candidate.</para>
    /// <para>尝试加载语言候选对应的 .po 文件。</para>
    /// </summary>
    private bool TryLoadLanguage(string candidate)
    {
        var poFile = new VirtualPath(AssetsDirectory, "langs", $"{candidate}.po");
        return VirtualFileSystem.FileExists(poFile) &&
               LocalizationManager.LoadPoFile(VirtualFileSystem, poFile);
    }

    /// <summary>
    /// <para>Try to load the .ttf font for a language candidate.</para>
    /// <para>尝试加载语言候选对应的 .ttf 字体。</para>
    /// </summary>
    private bool TryLoadFont(string candidate)
    {
        var fontPath = new VirtualPath(AssetsDirectory, "fonts", $"{candidate}.ttf");
        if (!VirtualFileSystem.FileExists(fontPath))
        {
            return false;
        }

        var stream = VirtualFileSystem.OpenRead(fontPath);
        if (stream == null)
        {
            return false;
        }

        Theme.AddFont(stream);
        return true;
    }

    /// <summary>
    /// <para>Load the fallback font when no language font was found.</para>
    /// <para>没有找到语言字体时加载兜底字体。</para>
    /// </summary>
    private void LoadDefaultFont()
    {
        var defaultFontPath = new VirtualPath(AssetsDirectory, "fonts", "default.ttf");
        if (!VirtualFileSystem.FileExists(defaultFontPath))
        {
            return;
        }

        var stream = VirtualFileSystem.OpenRead(defaultFontPath);
        if (stream != null)
        {
            Theme.AddFont(stream);
        }
    }
}