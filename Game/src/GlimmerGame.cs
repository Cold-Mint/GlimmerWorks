using GlimmerWorks.Game.Scene;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Input;
using Myra;
using Myra.Graphics2D;
using Myra.Graphics2D.Brushes;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game;

public class GlimmerGame : Microsoft.Xna.Framework.Game
{
    private readonly GameAppContext _appContext = new();

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
        MyraEnvironment.TooltipDelayInMs = 15;
        MyraEnvironment.TooltipOffset = new Point(12, 10);
        MyraEnvironment.TooltipCreator = widget =>
        {
            if (string.IsNullOrEmpty(widget.Tooltip))
            {
                return null;
            }

            return new Panel
            {
                Background = new RoundedCornersSolidBrush(new Color(28, 28, 30, 245))
                    { Radius = _appContext.Theme.Radius },
                Padding = new Thickness(12, 8),
                Widgets =
                {
                    new Label
                    {
                        Text = widget.Tooltip,
                        TextColor = _appContext.Theme.OnPrimary,
                        Font = _appContext.Theme.GetFont(ThemeFontKind.Small)
                    }
                }
            };
        };
        //Solve the problem of being unable to input Chinese.
        //解决无法输入中文的问题。
        _appContext.Desktop.HasExternalTextInput = true;
        Window.TextInput += (s, a) => { _appContext.Desktop.OnChar(a.Character); };
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
        GraphicsDevice.Clear(scene?.GetBackgroundColor() ?? _appContext.Theme.Surface);
        _appContext.Desktop?.Render();
        scene?.Draw(GraphicsDevice, gameTime);
        base.Draw(gameTime);
    }
}