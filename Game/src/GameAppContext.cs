using System.Globalization;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Uniform;
using GlimmerWorks.Game.Mods;
using GlimmerWorks.Game.Scene;
using GlimmerWorks.Game.Ui;
using AppContext = GlimmerWorks.Engine.AppContext;
using Myra.Graphics2D.UI;
using Microsoft.Extensions.Localization;
using SDL3;

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

    /// <summary>
    /// <para>The GPU device used by the game.</para>
    /// <para>游戏使用的 GPU 设备。</para>
    /// </summary>
    public IntPtr GpuDevice { get; set; }

    /// <summary>
    /// <para>The game window.</para>
    /// <para>游戏窗口。</para>
    /// </summary>
    public IntPtr Window { get; set; }

    /// <summary>
    /// <para>The swapchain texture format.</para>
    /// <para>交换链纹理格式。</para>
    /// </summary>
    public SDL.GPUTextureFormat SwapchainFormat { get; set; }

    /// <summary>
    /// <para>The Myra UI manager, created lazily once the GPU device and assets are available.</para>
    /// <para>Myra UI 管理器，在 GPU 设备和资产可用后延迟创建。</para>
    /// </summary>
    internal GameUi? Ui { get; private set; }

    private bool _uiInitializationErrorLogged;

    /// <summary>
    /// <para>InitializeUi</para>
    /// <para>使用已保存的设备和窗口信息初始化 Myra UI。</para>
    /// </summary>
    /// <returns></returns>
    public bool InitializeUi()
    {
        return InitializeUi(GpuDevice, Window, SwapchainFormat);
    }

    /// <summary>
    /// <para>EnsureUiInitialized</para>
    /// <para>确保 Myra UI 已初始化。</para>
    /// </summary>
    /// <param name="frame"></param>
    public void EnsureUiInitialized(in SceneFrameContext frame)
    {
        InitializeUi(frame.Device, frame.Window, frame.SwapchainFormat);
    }

    private bool InitializeUi(IntPtr device, IntPtr window, SDL.GPUTextureFormat swapchainFormat)
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
            if (!_uiInitializationErrorLogged)
            {
                _uiInitializationErrorLogged = true;
                LogCat.Error("Failed to create the Myra UI shaders.");
            }

            return false;
        }

        Ui = new GameUi(device, window, swapchainFormat, vertexShader, fragmentShader);
        Desktop = Ui.Desktop;
        LogCat.Info("Myra UI initialized.");
        return true;
    }

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

                Theme.AddFont(stream);
                loadFont = true;
            }

            if (loadFont && loadLangs)
            {
                break;
            }
        }

        if (!loadFont)
        {
            var defaultFontPath = new VirtualPath("assets", "fonts", "default.ttf");
            if (VirtualFileSystem.FileExists(defaultFontPath))
            {
                var stream = VirtualFileSystem.OpenRead(defaultFontPath);
                if (stream != null)
                {
                    Theme.AddFont(stream);
                }
            }
        }
    }
}