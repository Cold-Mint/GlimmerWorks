using System.Diagnostics;
using GlimmerWorks.Engine.Extensions;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.Vfs;
using GlimmerWorks.Game.Graphics.Rendering;
using Myra.Graphics2D;
using Myra.Graphics2D.UI;


namespace GlimmerWorks.Game.Scene;

public class SplashScene(GameAppContext appContext) : BaseScene(appContext)
{
    private const int MinimumDisplayMs = 1000;

    private readonly Stopwatch _displayStopwatch = Stopwatch.StartNew();
    private Label? _loadLabel;
    private string? _pendingMessage;
    private LogLevels _pendingLevel = LogLevels.Info;

    public override void OnCreate()
    {
        var panel = new Panel();
        _loadLabel = new Label
        {
            Text = AppContext.Localizer["Prepare"].Value,
            TextColor = AppContext.Theme.Info,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Bottom,
            Margin = new Thickness(0, 0, 0, 60),
            Font = AppContext.Theme.GetFont(ThemeFontKind.Small),
        };
        panel.Widgets.Add(_loadLabel);
        panel.Widgets.Add(new Label
        {
            Text = "GlimmerWorks",
            TextColor = AppContext.Theme.Info,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Center,
            Font = AppContext.Theme.GetFont(ThemeFontKind.Large),
        });
        SetWidget(panel);
        LoadPacks();
    }

    protected override void OnDraw(Renderer2D renderer)
    {
        if (_pendingMessage == null || _loadLabel == null)
        {
            return;
        }

        _loadLabel.Text = _pendingMessage;
        _loadLabel.TextColor = _pendingLevel switch
        {
            LogLevels.Info => AppContext.Theme.Info,
            LogLevels.Warn => AppContext.Theme.Warn,
            LogLevels.Error => AppContext.Theme.Error,
            _ => AppContext.Theme.Info
        };
        _pendingMessage = null;
    }

    private async void LoadPacks()
    {
        var dataPackCount =
            await AppContext.DataPackManager.ScanAndLoad(AppContext,
                new VirtualPath(AppContext.GameConfig.DataPackPath.SplitPath()), OnPackMessage);
        if (dataPackCount < 0)
        {
            LogCat.Error("Data pack scan failed.");
            return;
        }

        
        OnPackMessage(
            new PackMessage(AppContext.Localizer["Loaded {0} data packs", dataPackCount].Value));
        if (dataPackCount == 0)
        {
            LogCat.Error("No data packs found.");
            return;
        }

        var dimensionResources = AppContext.DataRepository.FirstOrDefaultResources<DimensionResource>(Filter);
        if (dimensionResources == null)
        {
            OnPackMessage(
                new PackMessage(AppContext.Localizer["At least one initial dimension is required."].Value)
                {
                    Level = LogLevels.Error
                });
            return;
        }

        var assetsPackCount =
            await AppContext.AssetsPackManager.ScanAndLoad(AppContext,
                new VirtualPath(AppContext.GameConfig.AssetsPackPath.SplitPath()), OnPackMessage);
        if (assetsPackCount < 0)
        {
            LogCat.Error("Asset pack scan failed.");
            return;
        }

        OnPackMessage(
            new PackMessage(AppContext.Localizer["Loaded {0} asset packs", assetsPackCount].Value));
        if (assetsPackCount == 0)
        {
            LogCat.Error("No asset packs found.");
            return;
        }

        if (!AppContext.InitializeUi())
        {
            LogCat.Error("Failed to initialize the Myra UI.");
        }

        var remainingDisplayMs = MinimumDisplayMs - (int)_displayStopwatch.ElapsedMilliseconds;
        if (remainingDisplayMs > 0)
        {
            await Task.Delay(remainingDisplayMs);
        }

        AppContext.SceneStack.AddTask(
            new SceneStackTask { Scene = new MainScene(AppContext), Type = StackType.Replace });
    }

    private static bool Filter(DimensionResource dimensionResource)
    {
        return dimensionResource.AllowAsStarting;
    }


    private void OnPackMessage(PackMessage packMessage)
    {
        var message = AppContext.Localizer[packMessage.Name, packMessage.Args].Value;
        if (packMessage.Path != null)
        {
            message = AppContext.Localizer["In {0}: {1}",
                packMessage.Path.Path, message].Value;
        }

        switch (packMessage.Level)
        {
            case LogLevels.Info:
                LogCat.Info(message);
                break;
            case LogLevels.Warn:
                LogCat.Warn(message);
                break;
            case LogLevels.Error:
                LogCat.Error(message);
                break;
        }

        _pendingMessage = message;
        _pendingLevel = packMessage.Level;
    }
}