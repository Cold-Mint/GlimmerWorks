using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Engine.Scene;

/// <summary>
/// Scene base class: defines the lifecycle contract of a scene.
/// 场景基类：定义场景的生命周期契约。
/// </summary>
public abstract class Scene(AppContext appContext)
{
    private Widget? _widget;

    protected AppContext AppContext => appContext;

    /// <summary>
    /// <para>SetWidget</para>
    /// <para>设置组件</para>
    /// </summary>
    /// <param name="widget"></param>
    protected void SetWidget(Widget? widget)
    {
        _widget = widget;
    }


    public virtual void Update(GameTime gameTime)
    {
        if (AppContext.SceneStack.IsTopScene(this) && appContext.Desktop?.Root != _widget)
        {
            appContext.Desktop?.Root = _widget;
        }
    }

    public virtual void Draw(GraphicsDevice graphicsDevice, GameTime gameTime)
    {
    }

    /// <summary>
    /// <para>GetBackgroundColor</para>
    /// <para>获取场景背景色</para>
    /// </summary>
    /// <returns></returns>
    public virtual Color? GetBackgroundColor()
    {
        return null;
    }

    public virtual void OnCreate()
    {
    }

    public virtual void OnDestroy()
    {
    }

    public virtual void OnPauseScene()
    {
    }

    public virtual void OnResumeScene()
    {
    }

    /// <summary>
    /// This method is called when the back key is pressed. On Android systems, navigate up; on desktop platforms, press ESC.
    /// 当返回键被按下时调用这个方法，安卓系统向上导航，桌面平台按下ESC。
    /// </summary>
    /// <returns>If true is returned, the framework considers the scene has handled the back event by itself. 如果返回true，告诉框架场景自行处理了返回事件。</returns>
    public virtual bool OnBackPressed()
    {
        return false;
    }
}