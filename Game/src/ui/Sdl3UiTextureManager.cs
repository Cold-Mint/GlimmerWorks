using System.Drawing;
using FontStashSharp.Interfaces;
using GlimmerWorks.Engine.Log;
using SDL3;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>An <see cref="ITexture2DManager"/> implementation backed by SDL3 GPU textures.</para>
/// <para>基于 SDL3 GPU 纹理的 <see cref="ITexture2DManager"/> 实现，用于 Myra/FontStashSharp。</para>
/// </summary>
internal sealed class Sdl3UiTextureManager(IntPtr device) : ITexture2DManager
{
    internal sealed class TextureUpload(Sdl3UiTexture texture, Rectangle bounds, byte[] data)
    {
        public Sdl3UiTexture Texture { get; } = texture;
        public Rectangle Bounds { get; } = bounds;
        public byte[] Data { get; } = data;
    }

    private readonly List<Sdl3UiTexture> _textures = [];
    private readonly List<TextureUpload> _pendingUploads = [];

    internal IReadOnlyList<Sdl3UiTexture> Textures => _textures;

    internal List<TextureUpload> PendingUploads => _pendingUploads;

    public object CreateTexture(int width, int height)
    {
        width = Math.Max(1, width);
        height = Math.Max(1, height);

        var createInfo = new SDL.GPUTextureCreateInfo
        {
            Type = SDL.GPUTextureType.TextureType2D,
            Format = SDL.GPUTextureFormat.R8G8B8A8Unorm,
            Usage = SDL.GPUTextureUsageFlags.Sampler,
            Width = (uint)width,
            Height = (uint)height,
            LayerCountOrDepth = 1,
            NumLevels = 1,
            SampleCount = SDL.GPUSampleCount.SampleCount1
        };

        var handle = SDL.CreateGPUTexture(device, createInfo);
        if (handle == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create UI texture ({width}x{height}): {SDL.GetError()}");
        }

        var texture = new Sdl3UiTexture(handle, width, height);
        _textures.Add(texture);
        return texture;
    }

    public Point GetTextureSize(object texture) => ((Sdl3UiTexture)texture).Size;

    public void SetTextureData(object texture, Rectangle bounds, byte[] data)
    {
        if (texture is not Sdl3UiTexture uiTexture || uiTexture.Handle == IntPtr.Zero)
        {
            return;
        }

        if (bounds.Width <= 0 || bounds.Height <= 0)
        {
            return;
        }

        var requiredBytes = bounds.Width * bounds.Height * 4;
        if (data.Length < requiredBytes)
        {
            LogCat.Warn($"UI texture upload is too small. Expected {requiredBytes} bytes, got {data.Length}.");
            return;
        }

        // Copy because the caller may reuse its scratch buffer before the upload is flushed.
        // 复制数据，因为调用方可能在真正上传前复用临时缓冲区。
        var payload = new byte[requiredBytes];
        Array.Copy(data, payload, requiredBytes);
        _pendingUploads.Add(new TextureUpload(uiTexture, bounds, payload));
    }
}
