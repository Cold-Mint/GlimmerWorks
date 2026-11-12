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
            Text = AppContext.LocalizationManager.Get("ui.prepare"),
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
            new PackMessage(AppContext.LocalizationManager.GetFormat("ui.load_data_pack_count", dataPackCount)));

        var assetsPackLoader = new AssetsPackLoader();
        var assetsPackCount =
            await assetsPackLoader.Scan(AppContext, new VirtualPath("mods", "resource_packs"), OnPackMessage);
        OnPackMessage(
            new PackMessage(AppContext.LocalizationManager.GetFormat("ui.load_assets_pack_count", assetsPackCount)));
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
            message = AppContext.LocalizationManager.GetFormat("ui.at_source_file",
                packMessage.Path.Path, packMessage.Message);
        }

        switch (packMessage.Level)
        {
            case LogLevel.Info:
                _loadLabel?.TextColor = AppContext.Theme.Info;
                AppContext.LogCat.Info(message);
                break;
            case LogLevel.Warn:
                _loadLabel?.TextColor = AppContext.Theme.Warn;
                AppContext.LogCat.Warn(message);
                break;
            case LogLevel.Error:
                _loadLabel?.TextColor = AppContext.Theme.Error;
                AppContext.LogCat.Error(message);
                break;
        }

        _loadLabel?.Text = message;
    }
}