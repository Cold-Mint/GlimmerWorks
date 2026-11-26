using System.Buffers.Binary;
using System.Drawing;
using System.Numerics;
using System.Runtime.InteropServices;
using FontStashSharp.Interfaces;
using GlimmerWorks.Engine.Log;
using Myra.Graphics2D;
using Myra.Platform;
using SDL3;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>Myra renderer implementation backed by the SDL3 GPU API.</para>
/// <para>基于 SDL3 GPU API 的 Myra 渲染器实现。</para>
/// </summary>
internal sealed class Sdl3UiRenderer : IMyraRenderer
{
    private const int VertexSize = 32;
    private const int MaxVertices = 65536;

    private readonly IntPtr _device;
    private readonly IntPtr _pipeline;
    private readonly IntPtr _vertexBuffer;
    private readonly IntPtr _nearestSampler;
    private readonly IntPtr _linearSampler;
    private readonly Sdl3UiTextureManager _textureManager;
    private readonly List<DrawCommand> _commands = [];

    private readonly UiVertex[] _vertices = new UiVertex[MaxVertices];
    private byte[] _vertexUploadBuffer = [];
    private int _vertexCount;
    private DrawCommand? _currentCommand;
    private Rectangle _scissor = new(0, 0, int.MaxValue, int.MaxValue);
    private TextureFiltering _filtering = TextureFiltering.Nearest;
    private IntPtr _commandBuffer;
    private bool _disposed;

    public Sdl3UiRenderer(IntPtr device, IntPtr pipeline)
    {
        _device = device;
        _pipeline = pipeline;
        _textureManager = new Sdl3UiTextureManager(device);

        _nearestSampler = CreateSampler(SDL.GPUFilter.Nearest);
        _linearSampler = CreateSampler(SDL.GPUFilter.Linear);

        var vertexBufferInfo = new SDL.GPUBufferCreateInfo
        {
            Usage = SDL.GPUBufferUsageFlags.Vertex,
            Size = MaxVertices * VertexSize
        };
        _vertexBuffer = SDL.CreateGPUBuffer(device, vertexBufferInfo);
        if (_vertexBuffer == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create the Myra vertex buffer: {SDL.GetError()}");
        }
    }

    public ITexture2DManager TextureManager => _textureManager;

    public RendererType RendererType => RendererType.Quad;

    public Rectangle Scissor
    {
        get => _scissor;
        set
        {
            _scissor = value;
            _currentCommand = null;
        }
    }

    public void Begin(TextureFiltering textureFiltering)
    {
        _filtering = textureFiltering;
        _currentCommand = null;
    }

    public void End()
    {
        _currentCommand = null;
    }

    public void DrawSprite(object texture, Vector2 pos, Rectangle? src, FontStashSharp.FSColor color,
        float rotation, Vector2 scale, float depth)
    {
        throw new NotSupportedException("The SDL3 Myra backend uses quad rendering.");
    }

    public void DrawQuad(object texture, ref VertexPositionColorTexture topLeft,
        ref VertexPositionColorTexture topRight, ref VertexPositionColorTexture bottomLeft,
        ref VertexPositionColorTexture bottomRight)
    {
        if (texture is not Sdl3UiTexture uiTexture || uiTexture.Handle == IntPtr.Zero)
        {
            return;
        }

        if (_vertexCount + 6 > _vertices.Length)
        {
            LogCat.Warn("The Myra vertex buffer is full; further UI geometry was skipped.");
            return;
        }

        if (_currentCommand == null || _currentCommand.Texture != uiTexture.Handle ||
            _currentCommand.Scissor != _scissor || _currentCommand.Filtering != _filtering)
        {
            _currentCommand = new DrawCommand
            {
                Texture = uiTexture.Handle,
                FirstVertex = _vertexCount,
                Scissor = _scissor,
                Filtering = _filtering
            };
            _commands.Add(_currentCommand);
        }

        AddVertex(topLeft);
        AddVertex(topRight);
        AddVertex(bottomLeft);
        AddVertex(bottomRight);
        AddVertex(bottomLeft);
        AddVertex(topRight);

        _currentCommand.VertexCount += 6;
    }

    /// <summary>
    /// <para>Resets the geometry of the previous frame before Myra records a new one.</para>
    /// <para>在 Myra 记录新一帧的几何数据前，清空上一帧的数据。</para>
    /// </summary>
    public void BeginFrame()
    {
        _vertexCount = 0;
        _commands.Clear();
        _currentCommand = null;
    }

    /// <summary>
    /// <para>Uploads queued texture data and the UI vertex buffer, then pushes the projection uniform.</para>
    /// <para>上传排队的纹理数据与 UI 顶点缓冲，并推送投影常量。</para>
    /// </summary>
    public void PrepareFrame(IntPtr commandBuffer)
    {
        _commandBuffer = commandBuffer;
        UploadPendingTextures(commandBuffer);
        UploadVertexBuffer(commandBuffer);
    }

    /// <summary>
    /// <para>Uploads every queued UI texture to the GPU.</para>
    /// <para>把排队中的 UI 纹理上传到 GPU。</para>
    /// </summary>
    private void UploadPendingTextures(IntPtr commandBuffer)
    {
        var uploads = _textureManager.PendingUploads;
        if (uploads.Count == 0)
        {
            return;
        }

        var textureBytes = 0;
        foreach (var upload in uploads)
        {
            textureBytes += upload.Data.Length;
        }

        var transferBuffer = CreateUploadTransferBuffer(textureBytes, "UI texture");
        if (transferBuffer == IntPtr.Zero)
        {
            return;
        }

        var mapped = SDL.MapGPUTransferBuffer(_device, transferBuffer, false);
        if (mapped == IntPtr.Zero)
        {
            LogCat.Error($"Failed to map the UI texture transfer buffer: {SDL.GetError()}");
            SDL.ReleaseGPUTransferBuffer(_device, transferBuffer);
            uploads.Clear();
            return;
        }

        var copies = CopyTextureUploads(mapped, uploads);
        SDL.UnmapGPUTransferBuffer(_device, transferBuffer);

        var copyPass = SDL.BeginGPUCopyPass(commandBuffer);
        UploadTextureCopies(copyPass, transferBuffer, copies);
        SDL.EndGPUCopyPass(copyPass);
        SDL.ReleaseGPUTransferBuffer(_device, transferBuffer);
        uploads.Clear();
    }

    /// <summary>
    /// <para>Uploads the recorded UI vertex data.</para>
    /// <para>上传已记录的 UI 顶点数据。</para>
    /// </summary>
    private void UploadVertexBuffer(IntPtr commandBuffer)
    {
        var vertexBytes = _vertexCount * VertexSize;
        if (vertexBytes <= 0 || _vertexBuffer == IntPtr.Zero)
        {
            return;
        }

        var transferBuffer = CreateUploadTransferBuffer(vertexBytes, "UI vertex");
        if (transferBuffer == IntPtr.Zero)
        {
            return;
        }

        var mapped = SDL.MapGPUTransferBuffer(_device, transferBuffer, false);
        if (mapped == IntPtr.Zero)
        {
            LogCat.Error($"Failed to map the UI vertex transfer buffer: {SDL.GetError()}");
            SDL.ReleaseGPUTransferBuffer(_device, transferBuffer);
            return;
        }

        var vertexData = MemoryMarshal.AsBytes(_vertices.AsSpan(0, _vertexCount));
        if (_vertexUploadBuffer.Length < vertexBytes)
        {
            _vertexUploadBuffer = new byte[vertexBytes];
        }

        vertexData.CopyTo(_vertexUploadBuffer);
        Marshal.Copy(_vertexUploadBuffer, 0, mapped, vertexBytes);

        SDL.UnmapGPUTransferBuffer(_device, transferBuffer);

        var copyPass = SDL.BeginGPUCopyPass(commandBuffer);
        var sourceInfo = new SDL.GPUTransferBufferLocation
        {
            TransferBuffer = transferBuffer,
            Offset = 0
        };
        var destinationInfo = new SDL.GPUBufferRegion
        {
            Buffer = _vertexBuffer,
            Offset = 0,
            Size = (uint)vertexBytes
        };
        SDL.UploadToGPUBuffer(copyPass, in sourceInfo, in destinationInfo, true);
        SDL.EndGPUCopyPass(copyPass);
        SDL.ReleaseGPUTransferBuffer(_device, transferBuffer);
    }

    private IntPtr CreateUploadTransferBuffer(int size, string label)
    {
        var createInfo = new SDL.GPUTransferBufferCreateInfo
        {
            Usage = SDL.GPUTransferBufferUsage.Upload,
            Size = (uint)size
        };
        var transferBuffer = SDL.CreateGPUTransferBuffer(_device, createInfo);
        if (transferBuffer == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create the {label} transfer buffer: {SDL.GetError()}");
        }

        return transferBuffer;
    }

    private static List<TextureCopy> CopyTextureUploads(IntPtr mapped,
        List<Sdl3UiTextureManager.TextureUpload> uploads)
    {
        var copies = new List<TextureCopy>(uploads.Count);
        var offset = 0;
        foreach (var upload in uploads)
        {
            Marshal.Copy(upload.Data, 0, IntPtr.Add(mapped, offset), upload.Data.Length);
            copies.Add(new TextureCopy(upload.Texture, upload.Bounds, offset));
            offset += upload.Data.Length;
        }

        return copies;
    }

    private static void UploadTextureCopies(IntPtr copyPass, IntPtr transferBuffer,
        List<TextureCopy> copies)
    {
        foreach (var copy in copies)
        {
            var source = new SDL.GPUTextureTransferInfo
            {
                TransferBuffer = transferBuffer,
                Offset = (uint)copy.Offset,
                PixelsPerRow = (uint)copy.Bounds.Width,
                RowsPerLayer = (uint)copy.Bounds.Height
            };
            var destination = new SDL.GPUTextureRegion
            {
                Texture = copy.Texture.Handle,
                MipLevel = 0,
                Layer = 0,
                X = (uint)copy.Bounds.X,
                Y = (uint)copy.Bounds.Y,
                Z = 0,
                W = (uint)copy.Bounds.Width,
                H = (uint)copy.Bounds.Height,
                D = 1
            };
            SDL.UploadToGPUTexture(copyPass, in source, in destination, false);
        }
    }

    /// <summary>
    /// <para>Draws the recorded UI geometry into an already active render pass.</para>
    /// <para>将已记录的 UI 几何数据绘制到已开启的渲染通道中。</para>
    /// </summary>
    public void Draw(IntPtr renderPass, int width, int height)
    {
        if (_vertexCount == 0 || _pipeline == IntPtr.Zero || _vertexBuffer == IntPtr.Zero)
        {
            return;
        }

        SDL.BindGPUGraphicsPipeline(renderPass, _pipeline);

        Span<byte> viewSize = stackalloc byte[16];
        BinaryPrimitives.WriteSingleLittleEndian(viewSize, width);
        BinaryPrimitives.WriteSingleLittleEndian(viewSize[4..], height);
        SDL.PushGPUVertexUniformData(_commandBuffer, 0, viewSize, (uint)viewSize.Length);

        var viewport = new SDL.GPUViewport
        {
            X = 0,
            Y = 0,
            W = width,
            H = height,
            MinDepth = 0,
            MaxDepth = 1
        };
        SDL.SetGPUViewport(renderPass, in viewport);

        var vertexBinding = new SDL.GPUBufferBinding
        {
            Buffer = _vertexBuffer,
            Offset = 0
        };
        SDL.BindGPUVertexBuffers(renderPass, 0, [vertexBinding], 1);

        var bounds = new Rectangle(0, 0, width, height);
        foreach (var command in _commands)
        {
            if (command.VertexCount <= 0)
            {
                continue;
            }

            var scissor = Rectangle.Intersect(command.Scissor, bounds);
            if (scissor.Width <= 0 || scissor.Height <= 0)
            {
                continue;
            }

            var scissorRect = new SDL.Rect
            {
                X = scissor.X,
                Y = scissor.Y,
                W = scissor.Width,
                H = scissor.Height
            };
            SDL.SetGPUScissor(renderPass, in scissorRect);

            var sampler = command.Filtering == TextureFiltering.Linear ? _linearSampler : _nearestSampler;
            var textureBinding = new SDL.GPUTextureSamplerBinding
            {
                Texture = command.Texture,
                Sampler = sampler
            };
            SDL.BindGPUFragmentSamplers(renderPass, 0, [textureBinding], 1);

            SDL.DrawGPUPrimitives(renderPass, (uint)command.VertexCount, 1, (uint)command.FirstVertex, 0);
        }
    }

    public void Dispose()
    {
        if (_disposed)
        {
            return;
        }

        _disposed = true;
        foreach (var handle in _textureManager.Textures
                     .Select(static texture => texture.Handle)
                     .Where(static handle => handle != IntPtr.Zero))
        {
            SDL.ReleaseGPUTexture(_device, handle);
        }

        if (_vertexBuffer != IntPtr.Zero)
        {
            SDL.ReleaseGPUBuffer(_device, _vertexBuffer);
        }

        if (_nearestSampler != IntPtr.Zero)
        {
            SDL.ReleaseGPUSampler(_device, _nearestSampler);
        }

        if (_linearSampler != IntPtr.Zero)
        {
            SDL.ReleaseGPUSampler(_device, _linearSampler);
        }
    }

    private IntPtr CreateSampler(SDL.GPUFilter filter)
    {
        var createInfo = new SDL.GPUSamplerCreateInfo
        {
            MinFilter = filter,
            MagFilter = filter,
            MipmapMode = SDL.GPUSamplerMipmapMode.Nearest,
            AddressModeU = SDL.GPUSamplerAddressMode.ClampToEdge,
            AddressModeV = SDL.GPUSamplerAddressMode.ClampToEdge,
            AddressModeW = SDL.GPUSamplerAddressMode.ClampToEdge,
            MipLodBias = 0,
            MaxAnisotropy = 1,
            MinLod = 0,
            MaxLod = 0,
            EnableAnisotropy = false,
            EnableCompare = false
        };

        var sampler = SDL.CreateGPUSampler(_device, createInfo);
        if (sampler == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create a UI sampler: {SDL.GetError()}");
        }

        return sampler;
    }

    private void AddVertex(VertexPositionColorTexture vertex)
    {
        if (_vertexCount >= _vertices.Length)
        {
            return;
        }

        _vertices[_vertexCount++] = new UiVertex
        {
            Position = new Vector2(vertex.Position.X, vertex.Position.Y),
            TexCoord = vertex.TextureCoordinate,
            Color = new Vector4(vertex.Color.R / 255.0f, vertex.Color.G / 255.0f,
                vertex.Color.B / 255.0f, vertex.Color.A / 255.0f)
        };
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct UiVertex
    {
        public Vector2 Position;
        public Vector2 TexCoord;
        public Vector4 Color;
    }

    private readonly record struct TextureCopy(Sdl3UiTexture Texture, Rectangle Bounds, int Offset);

    private sealed class DrawCommand
    {
        public IntPtr Texture { get; init; }
        public int FirstVertex { get; init; }
        public int VertexCount { get; set; }
        public Rectangle Scissor { get; init; }
        public TextureFiltering Filtering { get; init; }
    }
}