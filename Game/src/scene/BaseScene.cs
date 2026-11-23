using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Myra.Graphics2D.UI;

namespace GlimmerWorks.Game.Scene;

/// <summary>
/// Scene base class: defines the lifecycle contract of a scene.
/// 场景基类：定义场景的生命周期契约。
/// </summary>
public abstract class BaseScene(GameAppContext appContext)
{
    private Widget? _widget;

    protected GameAppContext AppContext => appContext;

    /// <summary>
    /// <para>When the scene is pushed onto the stack, the request code set by the parent scene is used.</para>
    /// <para>当场景Push到栈内后，由上级场景设置的请求码</para>
    /// </summary>
    public int RequestCode { get; set; }

    /// <summary>
    /// <para>The result obtained after returning from this scene before being posted by Pop</para>
    /// <para>当此场景被Pop出去前返回的结果</para>
    /// </summary>
    /// <returns></returns>
    public virtual object? Result()
    {
        return null;
    }

    /// <summary>
    /// <para>Receive the results from the topmost scene. (If the top of the stack changes, the upper layer can optionally return the results.)</para>
    /// <para>接收来自栈顶场景的结果。（当栈顶发生变化时，可选的由上层返回结果）</para>
    /// </summary>
    /// <param name="requestCode">
    ///<para>requestCode</para>
    ///<para>请求代码</para>
    /// </param>
    /// <param name="result">
    ///<para>result</para>
    ///<para>结果</para>
    /// </param>
    public virtual void OnResult(int requestCode, object? result)
    {
    }

    /// <summary>
    /// <para>SetWidget</para>
    /// <para>设置组件</para>
    /// </summary>
    /// <param name="widget"></param>
    protected void SetWidget(Widget? widget)
    {
        _widget = widget;
    }


    /// <summary>
    /// <para>Update</para>
    /// <para>更新</para>
    /// </summary>
    /// <param name="gameTime"></param>
    public virtual void Update(GameTime gameTime)
    {
        if (AppContext.SceneStack.IsTopScene(this) && appContext.Desktop?.Root != _widget)
        {
            appContext.Desktop?.Root = _widget;
        }
    }

    /// <summary>
    /// <para>Draw</para>
    /// <para>绘制</para>
    /// </summary>
    /// <param name="graphicsDevice">
    ///<para>graphicsDevice</para>
    ///<para>图像设备</para>
    /// </param>
    /// <param name="gameTime">
    ///<para>gameTime</para>
    ///<para>游戏时间</para>
    /// </param>
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

    /// <summary>
    /// <para>OnCreate</para>
    /// <para>当创建场景后</para>
    /// </summary>
    public virtual void OnCreate()
    {
    }


    /// <summary>
    /// <para>OnDestroy</para>
    /// <para>当从场景栈内弹出时</para>
    /// </summary>
    public virtual void OnDestroy()
    {
    }

    /// <summary>
    /// <para>OnPauseScene</para>
    /// <para>当场景暂停时</para>
    /// </summary>
    public virtual void OnPauseScene()
    {
    }

    /// <summary>
    /// <para>OnResumeScene</para>
    /// <para>当场景恢复时</para>
    /// </summary>
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