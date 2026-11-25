using GlimmerWorks.Engine.Log;
using GlimmerWorks.Game.Scene;
using SDL3;

namespace GlimmerWorks.Game;

public class GlimmerGame
{
    private readonly GameAppContext _appContext = new();
    private IntPtr GpuDevice { get; set; } = IntPtr.Zero;

    private IntPtr Window { get; set; } = IntPtr.Zero;

    public GlimmerGame()
    {
        if (!SDL.Init(SDL.InitFlags.Video))
        {
            LogCat.Error($"SDL could not initialize: {SDL.GetError()}");
        }

        _appContext.TickTask.OnTick += OnTick;
        _appContext.SceneStack.AddTask(new SceneStackTask
            { Scene = new TestScene(_appContext), Type = StackType.Push });
        _appContext.SceneStack.ExecuteTaskQueue();
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

                topScene.Input(sdlEvent);
            }

            RenderFrame(topScene);
        }

        _appContext.TickTask.Stop();
        SDL.ReleaseWindowFromGPUDevice(GpuDevice, Window);
        SDL.DestroyWindow(Window);
        SDL.DestroyGPUDevice(GpuDevice);
        SDL.Quit();
    }

    /// <summary>
    /// <para>RenderFrame</para>
    /// <para>渲染一帧画面</para>
    /// </summary>
    private void RenderFrame(BaseScene scene)
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
            scene.Draw();
        }

        SDL.SubmitGPUCommandBuffer(cmdBuffer);
    }
}