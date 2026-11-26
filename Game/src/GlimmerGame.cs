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
        GpuDevice = SDL.CreateGPUDevice(SDL.GPUShaderFormat.SPIRV, false, null);
        if (GpuDevice == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU device: {SDL.GetError()}");
            return;
        }

        Window = SDL.CreateWindow("GlimmerWorks",
            _appContext.GameConfig.Window.Width,
            _appContext.GameConfig.Window.Height,
            SDL.WindowFlags.Resizable);
        if (Window == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create window: {SDL.GetError()}");
            return;
        }

        if (!SDL.ClaimWindowForGPUDevice(GpuDevice, Window))
        {
            LogCat.Error($"Failed to claim window for GPU device: {SDL.GetError()}");
            return;
        }

        _swapchainFormat = SDL.GetGPUSwapchainTextureFormat(GpuDevice, Window);
        _appContext.GpuDevice = GpuDevice;
        _appContext.Window = Window;
        _appContext.SwapchainFormat = _swapchainFormat;
        SDL.StartTextInput(Window);
        _appContext.InitializeUi();
        _appContext.SceneStack.AddTask(new SceneStackTask
            { Scene = new SplashScene(_appContext), Type = StackType.Push });
        _appContext.SceneStack.ExecuteTaskQueue();
        _appContext.TickTask.Start();
        while (_appContext.SceneStack.Count > 0)
        {
            _appContext.SceneStack.ExecuteTaskQueue();
            var topScene = _appContext.SceneStack.TopScene();
            if (topScene == null)
            {
                break;
            }

            while (SDL.PollEvent(out var sdlEvent))
            {
                var eventType = (SDL.EventType)sdlEvent.Type;
                if (eventType == SDL.EventType.Quit)
                {
                    _appContext.SceneStack.AddTask(new SceneStackTask { Type = StackType.Clear });
                    break;
                }

                _appContext.Ui?.ProcessEvent(sdlEvent);
                topScene.Input(sdlEvent);
            }

            DrawFrame(topScene);
        }

        _appContext.TickTask.Stop();
        _appContext.Ui?.Dispose();
        SDL.StopTextInput(Window);
        SDL.ReleaseWindowFromGPUDevice(GpuDevice, Window);
        SDL.DestroyWindow(Window);
        SDL.DestroyGPUDevice(GpuDevice);
        SDL.Quit();
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