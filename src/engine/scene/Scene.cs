using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.Scene;

/// <summary>
/// Scene base class: defines the lifecycle contract of a scene.
/// 场景基类：定义场景的生命周期契约。
/// </summary>
public abstract class Scene
{
    private readonly AppContext _appContext;

    /// <summary>
    /// Initializes a new scene bound to the given application context.
    /// 使用给定的应用上下文初始化一个新场景。
    /// </summary>
    /// <param name="context">Application context. 应用上下文。</param>
    protected Scene(AppContext context)
    {
        _appContext = context;
    }

    /// <summary>
    /// Gets the application context.
    /// 获取应用上下文。
    /// </summary>
    protected AppContext GetAppContext() => _appContext;

    public virtual void InitSceneUi(Desktop desktop)
    {
    }

    public virtual void Update(GameTime gameTime)
    {
    }

    public virtual void Draw(GraphicsDevice graphicsDevice, GameTime gameTime)
    {
    }

    /// <summary>
    /// This method will be called when the scene is superimposed over the current scene.
    /// 当场景叠加在当前场景之上时，将调用此方法。
    /// The overlay scene display does not trigger this method.
    /// Overlay场景显示不会触发此方法。
    /// </summary>
    public virtual void OnPauseScene()
    {
    }

    /// <summary>
    /// When a scene above the current scene is popped up, the current scene is displayed.
    /// 当在场景之上的场景被弹出后，当前场景显示出来。
    /// The overlay scene hiding does not trigger this method.
    /// Overlay场景隐藏不会触发此方法。
    /// </summary>
    public virtual void OnResumeScene()
    {
    }

    /// <summary>
    /// This method is called when the back key is pressed. On Android systems, navigate up; on desktop platforms, press ESC.
    /// 当返回键被按下时调用这个方法，安卓系统向上导航，桌面平台按下ESC。
    /// </summary>
    /// <returns>If true is returned, the framework considers the scene has handled the back event by itself. 如果返回true，告诉框架场景自行处理了返回事件。</returns>
    public bool OnBackPressed()
    {
        return false;
    }
}