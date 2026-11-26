using System.Runtime.InteropServices;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Scene;using Myra;
using Myra.Graphics2D.UI;
using SDL3;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>Owns the Myra desktop, platform and SDL3 GPU renderer.</para>
/// <para>持有 Myra 的 Desktop、平台实现和 SDL3 GPU 渲染器。</para>
/// </summary>
internal sealed class GameUi : IDisposable
{
    private readonly GpuResourcesHolder _shaderHolder = new();
    private readonly Sdl3UiRenderer _renderer;
    private readonly Sdl3UiPlatform _platform;
    private bool _disposed;

    public GameUi(IntPtr device, IntPtr window, SDL.GPUTextureFormat swapchainFormat,
        CompiledShader vertexShader, CompiledShader fragmentShader)
    {
        _shaderHolder.GpuDevice = device;

        var pipeline = new Pipeline
        {
            BlendMode = BlendMode.Alpha,
            VertexShader = vertexShader,
            FragmentShader = fragmentShader
        };
        var pipelineHandle = _shaderHolder.AddGraphicsPipeline(pipeline, swapchainFormat,
            VertexLayout.PositionTextureColor) ?? IntPtr.Zero;

        _renderer = new Sdl3UiRenderer(device, pipelineHandle);
        _platform = new Sdl3UiPlatform(window, _renderer);

        MyraEnvironment.Platform = _platform;

        Desktop = new Desktop
        {
            // SDL text input events are forwarded through ProcessEvent.
            // 通过 ProcessEvent 转发 SDL 文本输入事件。
            HasExternalTextInput = true
        };
    }

    public Desktop Desktop { get; }

    public void SetRoot(Widget? widget)
    {
        if (!ReferenceEquals(Desktop.Root, widget))
        {
            Desktop.Root = widget;
        }
    }

    public void ProcessEvent(SDL.Event sdlEvent)
    {
        switch ((SDL.EventType)sdlEvent.Type)
        {
            case SDL.EventType.TextInput:
            {
                var text = Marshal.PtrToStringUTF8(sdlEvent.Text.Text);
                if (!string.IsNullOrEmpty(text))
                {
                    foreach (var character in text)
                    {
                        Desktop.OnChar(character);
                    }
                }

                break;
            }
            case SDL.EventType.MouseWheel:
                _platform.AddWheel(sdlEvent.Wheel.Y);
                break;
        }
    }

    public void Render(in SceneFrameContext frame)
    {
        if (Desktop.Root == null)
        {
            return;
        }

        _renderer.BeginFrame();
        Desktop.Render();
        _renderer.PrepareFrame(frame.CommandBuffer);

        ReadOnlySpan<SDL.GPUColorTargetInfo> colorTargets =
        [
            new()
            {
                Texture = frame.SwapchainTexture,
                LoadOp = SDL.GPULoadOp.Load,
                StoreOp = SDL.GPUStoreOp.Store
            }
        ];

        var renderPass = SDL.BeginGPURenderPass(frame.CommandBuffer, colorTargets, 1, IntPtr.Zero);
        if (renderPass == IntPtr.Zero)
        {
            LogCat.Error($"GameUi.Render: failed to begin UI render pass: {SDL.GetError()}");
            return;
        }

        try
        {
            _renderer.Draw(renderPass, (int)frame.Width, (int)frame.Height);
        }
        finally
        {
            SDL.EndGPURenderPass(renderPass);
        }
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        _disposed = true;
        _renderer.Dispose();
        _shaderHolder.Destroy();
        Desktop.Dispose();
    }
}
