using System.Runtime.InteropServices;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Pipeline;
using GlimmerWorks.Game.Scene;
using Myra;
using Myra.Graphics2D.UI;
using SDL3;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>Owns the Myra desktop, platform and SDL3 GPU renderer.</para>
/// <para>持有 Myra 的 Desktop、平台实现和 SDL3 GPU 渲染器。</para>
/// </summary>
internal sealed class MyraGameUi : IDisposable
{
    private readonly GpuResourcesHolder _shaderHolder = new();
    private readonly Sdl3UiRenderer _renderer;
    private readonly Sdl3UiPlatform _platform;
    private bool _disposed;

    public MyraGameUi(IntPtr device, IntPtr window, SDL.GPUTextureFormat swapchainFormat,
        GraphicsPipeline pipeline)
    {
        _shaderHolder.GpuDevice = device;
        var pipelineHandle = _shaderHolder.AddGraphicsPipeline(pipeline, swapchainFormat) ?? IntPtr.Zero;

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

    /// <summary>
    /// <para>Route one SDL event to the UI.</para>
    /// <para>把一个 SDL 事件交给 UI。</para>
    /// </summary>
    /// <returns>
    /// <para>True when the UI consumed the event; otherwise false.</para>
    /// <para>返回 true 表示 UI 已消费该事件，调用方不应再向下传递。</para>
    /// </returns>
    public bool ProcessEvent(SDL.Event sdlEvent)
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

                return true;
            }
            case SDL.EventType.KeyDown:
            case SDL.EventType.KeyUp:
                // The UI owns the keyboard while a modal widget is open or a widget has keyboard focus.
                // 有模态控件或键盘焦点时，键盘事件归 UI。
                return Desktop.HasModalWidget || Desktop.FocusedKeyboardWidget is not null;
            case SDL.EventType.MouseWheel:
                if (!IsPointerOverUi)
                {
                    return false;
                }

                _platform.AddWheel(sdlEvent.Wheel.Y);
                return true;
            case SDL.EventType.MouseButtonDown:
            case SDL.EventType.MouseButtonUp:
            case SDL.EventType.MouseMotion:
                return IsPointerOverUi;
            default:
                return false;
        }
    }

    private bool IsPointerOverUi => Desktop.HasModalWidget || Desktop.IsMouseOverGUI;

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