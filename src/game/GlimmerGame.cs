using GlimmerWorks.Engine.Scene;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using Myra;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.Game;

public class GlimmerGame : Microsoft.Xna.Framework.Game
{
    private readonly GraphicsDeviceManager _graphicsDeviceManager;
    private Desktop? _desktop;
    private readonly AppContext _appContext = new();

    public GlimmerGame()
    {
        _graphicsDeviceManager = new GraphicsDeviceManager(this)
        {
            PreferredBackBufferWidth = _appContext.GameConfig.Window.Width,
            PreferredBackBufferHeight = _appContext.GameConfig.Window.Height,
            IsFullScreen = _appContext.GameConfig.Window.Fullscreen,
            SynchronizeWithVerticalRetrace = _appContext.GameConfig.Window.Vsync,
        };
        IsMouseVisible = true;
        Window.AllowUserResizing = true;
        MyraEnvironment.Game = this;
    }

    protected override void Initialize()
    {
        base.Initialize();
        _desktop = new Desktop();
        _appContext.SceneStack.OnPushScene += OnPushScene;
        _appContext.SceneStack.PushScene(new SplashScene(_appContext));
    }

    private void OnPushScene(Scene obj)
    {
        if (_desktop == null)
        {
            return;
        }

        obj.InitSceneUi(_desktop);
    }


    protected override void Update(GameTime gameTime)
    {
        _desktop?.UpdateInput();
        var scene = _appContext.SceneStack.Current();
        if (scene != null)
        {
            if (Keyboard.GetState().IsKeyDown(Keys.Escape) && !scene.OnBackPressed())
            {
                _appContext.SceneStack.PopScene();
            }

            scene.Update(gameTime);
        }

        if (_appContext.SceneStack.Count == 0)
        {
            Exit();
        }

        base.Update(gameTime);
    }

    protected override void Draw(GameTime gameTime)
    {
        GraphicsDevice.Clear(Color.Green);
        _desktop?.Render();
        var scene = _appContext.SceneStack.Current();
        scene?.Draw(GraphicsDevice, gameTime);
        base.Draw(gameTime);
    }
}