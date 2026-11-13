using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.PackLoader;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.engine.Vfs;
using Microsoft.Xna.Framework;
using Myra.Graphics2D;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;


namespace GlimmerWorks.Game;

public class SplashScene(AppContext appContext) : Scene(appContext)
{
    private Label? _loadLabel;

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
            TextColor = Color.White,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Center,
            Font = AppContext.Theme.GetFont(ThemeFontKind.Large),
        });
        SetWidget(panel);
        LoadPacks();
    }

    private async void LoadPacks()
    {
        var dataPackLoader = new DataPackLoader();
        var dataPackCount =
            await dataPackLoader.Scan(AppContext, new VirtualPath("mods", "data_packs"), OnPackMessage);
        OnPackMessage(
            new PackMessage(AppContext.Localizer["Loaded {0} data packs", dataPackCount].Value));

        var assetsPackLoader = new AssetsPackLoader();
        var assetsPackCount =
            await assetsPackLoader.Scan(AppContext, new VirtualPath("mods", "resource_packs"), OnPackMessage);
        OnPackMessage(
            new PackMessage(AppContext.Localizer["Loaded {0} asset packs", assetsPackCount].Value));
        AppContext.SceneStack.AddTask(
            new SceneStackTask { Scene = new MainScene(AppContext), Type = StackType.Replace });
    }


    public override Color? GetBackgroundColor()
    {
        return AppContext.Theme.Primary;
    }

    private void OnPackMessage(PackMessage packMessage)
    {
        var message = packMessage.Message;
        if (packMessage.Path != null)
        {
            message = AppContext.Localizer["In {0}: {1}",
                packMessage.Path.Path, packMessage.Message].Value;
        }

        switch (packMessage.Level)
        {
            case LogLevel.Info:
                _loadLabel?.TextColor = AppContext.Theme.Info;
                LogCat.Info(message);
                break;
            case LogLevel.Warn:
                _loadLabel?.TextColor = AppContext.Theme.Warn;
                LogCat.Warn(message);
                break;
            case LogLevel.Error:
                _loadLabel?.TextColor = AppContext.Theme.Error;
                LogCat.Error(message);
                break;
        }

        _loadLabel?.Text = message;
    }
}