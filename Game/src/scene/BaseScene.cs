using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Rendering;
using Myra.Graphics2D.UI;
using SDL3;

namespace GlimmerWorks.Game.Scene;

/// <summary>
/// Scene base class: defines the lifecycle contract of a scene.
/// 场景基类：定义场景的生命周期契约。
/// </summary>
public abstract class BaseScene(GameAppContext appContext)
{
    private Widget? _widget;

    protected GameAppContext AppContext => appContext;

    protected readonly GpuResourcesHolder GpuResourcesHolder = new();

    private Renderer2D? _renderer;

    /// <summary>
    /// <para>The color used to clear the screen before drawing.</para>
    /// <para>绘制前清屏使用的颜色。</para>
    /// </summary>
    protected virtual Color ClearColor => new(13, 13, 20);

    private bool _needEnsureGpuResources = true;

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
    /// <para>OnTick</para>
    /// <para>当Tick时</para>
    /// </summary>
    /// <remarks>
    ///<para>It is recommended to update the data within the Tick function, and it is not advisable to operate the UI.</para>
    ///<para>建议在Tick函数内更新数据，不建议操作UI。</para>
    /// </remarks>
    public virtual void OnTick(long deltaMs, long globalTick)
    {
    }

    public virtual void Input(SDL.Event sdlEvent)
    {
    }

    /// <summary>
    /// <para>EnsureGpuResources</para>
    /// <para>确保GPU资源可用</para>
    /// </summary>
    /// <param name="sceneFrameContext"></param>
    public void DoEnsureGpuResources(in SceneFrameContext sceneFrameContext)
    {
        if (_needEnsureGpuResources)
        {
            EnsureGpuResources(sceneFrameContext);
            _needEnsureGpuResources = false;
        }
    }

    /// <summary>
    /// <para>EnsureGpuResources</para>
    /// <para>在这里初始化Gpu资源</para>
    /// </summary>
    /// <param name="sceneFrameContext"></param>
    protected virtual void EnsureGpuResources(in SceneFrameContext sceneFrameContext)
    {
    }

    /// <summary>
    /// <para>Draw</para>
    /// <para>绘制一帧：清屏、调用 <see cref="OnDraw"/> 并结束渲染通道。</para>
    /// </summary>
    public void Draw(in SceneFrameContext sceneFrameContext)
    {
        var renderer = _renderer ??= new Renderer2D(appContext, GpuResourcesHolder);
        renderer.Begin(sceneFrameContext, ClearColor);
        try
        {
            OnDraw(renderer);
        }
        finally
        {
            renderer.End();
        }

        if (_widget == null)
        {
            AppContext.Ui?.SetRoot(null);
            return;
        }

        AppContext.EnsureUiInitialized(sceneFrameContext);
        AppContext.Ui?.SetRoot(_widget);
        AppContext.Ui?.Render(sceneFrameContext);
    }

    /// <summary>
    /// <para>OnDraw</para>
    /// <para>在这里使用 <see cref="Renderer2D"/> 提交绘制内容。</para>
    /// </summary>
    protected virtual void OnDraw(Renderer2D renderer)
    {
    }

    /// <summary>
    /// <para>DoCreate</para>
    /// <para>初始化 UI 环境并调用 <see cref="OnCreate"/>。</para>
    /// </summary>
    public void DoCreate()
    {
        AppContext.InitializeUi();
        OnCreate();
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
        GpuResourcesHolder.Destroy();
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