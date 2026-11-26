using System.Diagnostics;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Game.Scene;
using SDL3;

namespace GlimmerWorks.Game;

public class GlimmerGame
{
    private readonly GameAppContext _appContext = new();
    private IntPtr GpuDevice { get; set; } = IntPtr.Zero;

    private IntPtr Window { get; set; } = IntPtr.Zero;
    private SDL.GPUTextureFormat _swapchainFormat;

    public GlimmerGame()
    {
        if (!SDL.Init(SDL.InitFlags.Video))
        {
            LogCat.Error($"SDL could not initialize: {SDL.GetError()}");
        }

        _appContext.TickTask.OnTick += OnTick;
    }

    private void OnTick(long deltaMs, long globalTick)
    {
        var topScene = _appContext.SceneStack.TopScene();
        topScene?.OnTick(deltaMs, globalTick);
    }

    public void Start()
    {
        if (!InitializeGraphics())
        {
            return;
        }

        StartSceneStack();
        RunMainLoop();
        Shutdown();
    }

    /// <summary>
    /// <para>Create the GPU device, window and swapchain resources.</para>
    /// <para>创建 GPU 设备、窗口与交换链资源。</para>
    /// </summary>
    /// <returns><c>true</c> when the graphics resources are ready.</returns>
    private bool InitializeGraphics()
    {
        GpuDevice = SDL.CreateGPUDevice(SDL.GPUShaderFormat.SPIRV, false, null);
        if (GpuDevice == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU device: {SDL.GetError()}");
            return false;
        }

        Window = SDL.CreateWindow("GlimmerWorks",
            _appContext.GameConfig.Window.Width,
            _appContext.GameConfig.Window.Height,
            SDL.WindowFlags.Resizable);
        if (Window == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create window: {SDL.GetError()}");
            return false;
        }

        if (!SDL.ClaimWindowForGPUDevice(GpuDevice, Window))
        {
            LogCat.Error($"Failed to claim window for GPU device: {SDL.GetError()}");
            return false;
        }

        ApplyPresentMode();
        _swapchainFormat = SDL.GetGPUSwapchainTextureFormat(GpuDevice, Window);
        _appContext.GpuDevice = GpuDevice;
        _appContext.Window = Window;
        _appContext.SwapchainFormat = _swapchainFormat;
        SDL.StartTextInput(Window);
        _appContext.InitializeUi();
        return true;
    }

    /// <summary>
    /// <para>Push the initial scene and start the game tick thread.</para>
    /// <para>压入初始场景并启动游戏 Tick 线程。</para>
    /// </summary>
    private void StartSceneStack()
    {
        _appContext.SceneStack.AddTask(new SceneStackTask
            { Scene = new SplashScene(_appContext), Type = StackType.Push });
        _appContext.SceneStack.ExecuteTaskQueue();
        _appContext.TickTask.Start();
    }

    /// <summary>
    /// <para>Run the main loop until the scene stack becomes empty.</para>
    /// <para>运行主循环，直到场景栈为空。</para>
    /// </summary>
    private void RunMainLoop()
    {
        var frameIntervalMs = GetFrameIntervalMs();
        var frameStopwatch = Stopwatch.StartNew();
        var nextFrameMs = 0.0;
        while (_appContext.SceneStack.Count > 0)
        {
            _appContext.SceneStack.ExecuteTaskQueue();
            var topScene = _appContext.SceneStack.TopScene();
            if (topScene == null)
            {
                break;
            }

            PollEvents(topScene);
            DrawFrame(topScene);
            LimitFrameRate(frameIntervalMs, frameStopwatch, ref nextFrameMs);
        }
    }

    private double GetFrameIntervalMs()
    {
        var frameRate = _appContext.GameConfig.Window.FrameRate;
        return frameRate > 0 ? 1000.0 / frameRate : 0;
    }

    /// <summary>
    /// <para>Forward pending SDL events to the UI and the active scene.</para>
    /// <para>把待处理的 SDL 事件转发给 UI 与当前场景。</para>
    /// </summary>
    private void PollEvents(BaseScene scene)
    {
        while (SDL.PollEvent(out var sdlEvent))
        {
            if ((SDL.EventType)sdlEvent.Type == SDL.EventType.Quit)
            {
                _appContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Clear });
                break;
            }

            _appContext.Ui?.ProcessEvent(sdlEvent);
            scene.Input(sdlEvent);
        }
    }

    /// <summary>
    /// <para>Wait until the next frame is due.</para>
    /// <para>等待到下一帧的时间点。</para>
    /// </summary>
    private static void LimitFrameRate(double frameIntervalMs, Stopwatch frameStopwatch,
        ref double nextFrameMs)
    {
        if (frameIntervalMs <= 0)
        {
            return;
        }

        nextFrameMs += frameIntervalMs;
        var nowMs = frameStopwatch.Elapsed.TotalMilliseconds;
        var sleepMs = nextFrameMs - nowMs;
        if (sleepMs > 0)
        {
            SDL.DelayPrecise((ulong)(sleepMs * 1_000_000));
        }
        else
        {
            // Frame timeout: The next frame is due immediately.
            // 帧超时：下一帧应立即开始。
            nextFrameMs = nowMs;
        }
    }

    /// <summary>
    /// <para>Release the graphics resources and stop the game tick thread.</para>
    /// <para>释放图形资源并停止游戏 Tick 线程。</para>
    /// </summary>
    private void Shutdown()
    {
        _appContext.TickTask.Stop();
        _appContext.Ui?.Dispose();
        SDL.StopTextInput(Window);
        SDL.ReleaseWindowFromGPUDevice(GpuDevice, Window);
        SDL.DestroyWindow(Window);
        SDL.DestroyGPUDevice(GpuDevice);
        SDL.Quit();
    }

    /// <summary>
    /// <para>ApplyPresentMode</para>
    /// <para>将 [window] 中配置的垂直同步设置应用到交换链。</para>
    /// </summary>
    private void ApplyPresentMode()
    {
        if (_appContext.GameConfig.Window.VSync)
        {
            // VSync is the default present mode of a newly claimed window.
            // 垂直同步是新认领窗口的默认呈现模式。
            return;
        }

        if (!SDL.WindowSupportsGPUPresentMode(GpuDevice, Window, SDL.GPUPresentMode.Immediate))
        {
            LogCat.Warn("Immediate present mode is not supported; vertical sync stays enabled.");
            return;
        }

        if (!SDL.SetGPUSwapchainParameters(GpuDevice, Window, SDL.GPUSwapchainComposition.SDR,
                SDL.GPUPresentMode.Immediate))
        {
            LogCat.Error($"Failed to set the present mode: {SDL.GetError()}");
            return;
        }

        LogCat.Info("Vertical sync disabled.");
    }

    /// <summary>
    /// <para>DrawFrame</para>
    /// <para>绘制一帧画面</para>
    /// </summary>
    private void DrawFrame(BaseScene scene)
    {
        var cmdBuffer = SDL.AcquireGPUCommandBuffer(GpuDevice);
        if (cmdBuffer == IntPtr.Zero)
        {
            return;
        }

        SDL.AcquireGPUSwapchainTexture(cmdBuffer, Window,
            out var swapchainTexture, out var swapchainWidth, out var swapchainHeight);
        if (swapchainTexture != IntPtr.Zero)
        {
            var sceneFrameContext = new SceneFrameContext(GpuDevice, Window, cmdBuffer, swapchainTexture,
                swapchainWidth, swapchainHeight, _swapchainFormat);
            scene.DoEnsureGpuResources(sceneFrameContext);
            scene.Draw(sceneFrameContext);
        }

        SDL.SubmitGPUCommandBuffer(cmdBuffer);
    }
}