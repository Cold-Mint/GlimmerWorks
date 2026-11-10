using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.PackLoader;
using GlimmerWorks.Engine.Scene;
using GlimmerWorks.engine.Vfs;
using Microsoft.Xna.Framework;
using Myra.Graphics2D;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;


namespace GlimmerWorks.Game;

public class SplashScene(AppContext context) : Scene(context)
{
    private Label? _loadLabel;

    public override void InitSceneUi(Desktop desktop)
    {
        var panel = new Panel();
        _loadLabel = new Label
        {
            Text = context.LocalizationManager.Get("ui.prepare"),
            TextColor = Color.White,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Bottom,
            Margin = new Thickness(0, 0, 0, 60),
            Font = context.FontSystem.GetFont(32)
        };
        panel.Widgets.Add(_loadLabel);
        panel.Widgets.Add(new Label
        {
            Text = "GlimmerWorks",
            TextColor = Color.White,
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Center,
            Font = context.FontSystem.GetFont(150)
        });
        desktop.Root = panel;
        LoadPacks();
    }

    private async void LoadPacks()
    {
        var dataPackLoader = new DataPackLoader();
        var dataPackCount = await dataPackLoader.Scan(context, new VirtualPath("mods", "data_packs"), OnPackMessage);
        
        OnPackMessage(new PackMessage(context.LocalizationManager.GetFormat("ui.load_data_pack_count", dataPackCount)));
    }

    private void OnPackMessage(PackMessage obj)
    {
        switch (obj.Level)
        {
            case PackMessageLevel.Info:
                _loadLabel?.TextColor = Color.White;
                break;
            case PackMessageLevel.Warning:
                _loadLabel?.TextColor = Color.Yellow;
                break;
            case PackMessageLevel.Error:
                _loadLabel?.TextColor = Color.Red;
                break;
        }

        _loadLabel?.Text = obj.Message;
    }
}