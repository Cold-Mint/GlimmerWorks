using GlimmerWorks.Engine;
using GlimmerWorks.Engine.Scene;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using Myra;
using Myra.Graphics2D.UI;
using AppContext = GlimmerWorks.Engine.AppContext;

namespace GlimmerWorks.Game;

public class GlimmerGame : Microsoft.Xna.Framework.Game
{
    private readonly AppContext _appContext = new();
    private readonly Color _defaultBackgroundColor = new(242, 242, 247, 255);

    public GlimmerGame()
    {
        _appContext.GraphicsDeviceManager = new GraphicsDeviceManager(this)
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
        _appContext.Desktop = new Desktop();
        _appContext.SceneStack.AddTask(
            new SceneStackTask { Scene = new SplashScene(_appContext), Type = StackType.Push });
    }


    protected override void Update(GameTime gameTime)
    {
        base.Update(gameTime);
        InputTracker.Update();
        var scene = _appContext.SceneStack.Current();
        if (scene != null)
        {
            if (InputTracker.IsKeyJustPressed(Keys.Escape) && !scene.OnBackPressed())
            {
                _appContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Pop });
            }

            scene.Update(gameTime);
        }

        _appContext.SceneStack.ExecuteTaskQueue();
        if (_appContext.SceneStack.Count == 0)
        {
            Exit();
        }
    }

    protected override void Draw(GameTime gameTime)
    {
        var scene = _appContext.SceneStack.Current();
        GraphicsDevice.Clear(scene?.GetBackgroundColor() ?? _defaultBackgroundColor);
        _appContext.Desktop?.Render();
        scene?.Draw(GraphicsDevice, gameTime);
        base.Draw(gameTime);
    }
}