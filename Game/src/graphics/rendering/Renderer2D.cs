using System.Buffers.Binary;
using System.Numerics;
using System.Diagnostics;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Game.Graphics.Pipeline;
using GlimmerWorks.Game.Graphics.Uniform;
using GlimmerWorks.Game.Graphics.Vertex;
using GlimmerWorks.Game.Scene;
using SDL3;

namespace GlimmerWorks.Game.Graphics.Rendering;

public sealed class Renderer2D(GameAppContext appContext, GpuResourcesHolder gpu)
{
    private const int MaxVertices = 65536;
    private const int MaxVertexStride = VertexLayout.MaxStride;

    private readonly byte[] _uiVertexData = new byte[MaxVertices * MaxVertexStride];
    private readonly byte[] _materialVertexData = new byte[MaxVertices * MaxVertexStride];
    private readonly List<RenderCommand> _commands = [];
    private readonly Dictionary<string, Material> _materialCache = [];
    private readonly Dictionary<GraphicsPipeline, nint> _pipelineCache = [];
    private readonly Dictionary<TextureData, Texture2D> _textureCache = [];
    private readonly List<Texture2D> _pendingTextures = [];

    private nint _uiPipelineHandle;
    private nint _uiVertexBuffer;
    private nint _materialVertexBuffer;
    private nint _sampler;
    private GraphicsPipeline? _uiPipeline;
    private VertexLayout? _uiLayout;
    private Texture2D? _whiteTexture;
    private bool _uiPipelineLoadFailed;

    private SceneFrameContext _frameContext;
    private Color _clearColor;
    private int _uiVertexCount;
    private int _materialVertexCount;
    private int _materialVertexByteCount;
    private RenderCommand? _currentPrimitiveCommand;
    private bool _initialized;
    private bool _begun;

    /// <summary>
    /// <para>Start a frame. Must be paired with <see cref="End"/>.</para>
    /// <para>开始一帧，必须与 <see cref="End"/> 成对调用。</para>
    /// </summary>
    public void Begin(in SceneFrameContext frame, Color clearColor)
    {
        if (_begun)
        {
            End();
        }

        _frameContext = frame;
        _clearColor = clearColor;
        _uiVertexCount = 0;
        _materialVertexCount = 0;
        _materialVertexByteCount = 0;
        _commands.Clear();
        _currentPrimitiveCommand = null;
        _begun = true;

        gpu.GpuDevice = frame.Device;
        RuntimeUniformState.ViewSize = new Vector2(frame.Width, frame.Height);
        // Wrap the clock to one hour so the float stays precise.
        // 把时钟限制在一小时内，保证 float 精度。
        RuntimeUniformState.TimeSeconds =
            (float)(Stopwatch.GetTimestamp() % (Stopwatch.Frequency * 3600L)) / Stopwatch.Frequency;
        EnsureInitialized();
    }

    /// <summary>
    /// <para>Finish the frame: upload pending data, clear the target and draw all queued geometry.</para>
    /// <para>结束一帧：上传待处理数据、清空目标并绘制所有已提交的几何体。</para>
    /// </summary>
    public void End()
    {
        if (!_begun)
        {
            return;
        }

        _begun = false;

        if (_frameContext.CommandBuffer == IntPtr.Zero || _frameContext.SwapchainTexture == IntPtr.Zero)
        {
            ResetFrame();
            return;
        }

        PrepareMaterialResources();
        UploadPendingTextures();
        UploadVertexBuffers();

        ReadOnlySpan<SDL.GPUColorTargetInfo> colorTargets =
        [
            new()
            {
                Texture = _frameContext.SwapchainTexture,
                LoadOp = SDL.GPULoadOp.Clear,
                StoreOp = SDL.GPUStoreOp.Store,
                ClearColor = ToFColor(_clearColor)
            }
        ];

        var renderPass = SDL.BeginGPURenderPass(_frameContext.CommandBuffer, colorTargets, 1, IntPtr.Zero);
        if (renderPass == IntPtr.Zero)
        {
            LogCat.Error($"Renderer2D: failed to begin a render pass: {SDL.GetError()}");
            ResetFrame();
            return;
        }

        try
        {
            DrawCommands(renderPass);
        }
        finally
        {
            SDL.EndGPURenderPass(renderPass);
            ResetFrame();
        }
    }

    /// <summary>
    /// <para>Load a material from the asset packs. The same reference returns the same instance.</para>
    /// <para>从资产包加载材质。相同引用会返回同一个实例。</para>
    /// </summary>
    public Material? LoadMaterial(ResourceRef reference)
    {
        var key = $"{reference.TargetPackId}:{reference.ResourceId}";
        if (_materialCache.TryGetValue(key, out var existing))
        {
            return existing;
        }

        var result = appContext.AssetsPackManager.FindAsset<Material>(appContext, reference);
        if (!result.Exist || result.Result == null)
        {
            LogCat.Error($"Renderer2D: failed to load material '{reference.ResourceId}'.");
            return null;
        }

        _materialCache[key] = result.Result;
        return result.Result;
    }

    /// <summary>
    /// <para>Draw a material into a destination rectangle.</para>
    /// <para>把材质绘制到目标矩形中。</para>
    /// </summary>
    /// <param name="material"></param>
    /// <param name="destination"></param>
    /// <param name="tint">
    ///<para>Optional per-draw tint. When omitted the material's current runtime tint is used.</para>
    ///<para>可选的逐次绘制染色。省略时使用材质当前的运行时染色。</para>
    /// </param>
    /// <param name="source">
    ///<para>Optional source rectangle in texture pixels.</para>
    ///<para>可选的纹理源矩形（像素坐标）。</para>
    /// </param>
    public void DrawMaterial(Material? material, RectF destination, Color? tint = null,
        RectF? source = null)
    {
        if (!_begun || material?.Texture == null || material.Pipeline == null ||
            destination.Width <= 0 || destination.Height <= 0)
        {
            return;
        }

        var texture = material.Texture;
        if (texture.Width <= 0 || texture.Height <= 0)
        {
            return;
        }

        var sourceX = 0f;
        var sourceY = 0f;
        var sourceWidth = (float)texture.Width;
        var sourceHeight = (float)texture.Height;
        if (source is { } sourceRect)
        {
            sourceX = sourceRect.X;
            sourceY = sourceRect.Y;
            sourceWidth = sourceRect.Width;
            sourceHeight = sourceRect.Height;
        }

        var layout = material.Pipeline.VertexLayout ?? VertexLayout.PositionTexture;
        if (sourceWidth <= 0 || sourceHeight <= 0 ||
            _materialVertexCount + 6 > MaxVertices ||
            _materialVertexByteCount + 6 * layout.Stride > _materialVertexData.Length)
        {
            if (_materialVertexCount + 6 > MaxVertices ||
                _materialVertexByteCount + 6 * layout.Stride > _materialVertexData.Length)
            {
                LogCat.Warn("Renderer2D: the material vertex buffer is full; further geometry was skipped.");
            }

            return;
        }

        var uvMin = new Vector2(sourceX / texture.Width, sourceY / texture.Height);
        var uvMax = new Vector2((sourceX + sourceWidth) / texture.Width,
            (sourceY + sourceHeight) / texture.Height);
        var firstVertexByteOffset = _materialVertexByteCount;

        AddMaterialQuad(
            new Vector2(destination.Left, destination.Top),
            new Vector2(destination.Right, destination.Top),
            new Vector2(destination.Right, destination.Bottom),
            new Vector2(destination.Left, destination.Bottom),
            uvMin, uvMax, layout);

        // A primitive drawn after this material must start a new batch.
        // 此材质之后绘制的图元必须开启新的批次。
        _currentPrimitiveCommand = null;

        if (_commands.Count > 0)
        {
            var last = _commands[^1];
            if (last.IsMaterial && ReferenceEquals(last.Material, material) &&
                Nullable.Equals(last.Tint, tint))
            {
                last.VertexCount += 6;
                return;
            }
        }

        _commands.Add(new RenderCommand
        {
            IsMaterial = true,
            Material = material,
            Tint = tint,
            FirstVertexByteOffset = firstVertexByteOffset,
            VertexCount = 6
        });
    }

    /// <summary>
    /// <para>Draw a line segment as a quad.</para>
    /// <para>把线段绘制成一个有厚度的四边形。</para>
    /// </summary>
    public void DrawLine(Vector2 start, Vector2 end, Color color, float thickness = 1f)
    {
        if (!_begun || _whiteTexture == null)
        {
            return;
        }

        var delta = end - start;
        var length = delta.Length();
        if (length <= 0.0001f)
        {
            return;
        }

        var half = MathF.Max(thickness, 0.01f) * 0.5f;
        var normal = new Vector2(-delta.Y, delta.X) / length * half;
        var uv = new Vector2(0.5f, 0.5f);
        AddUiQuad(_whiteTexture,
            new UiQuad(start + normal, end + normal, end - normal, start - normal),
            uv, uv, ToVector4(color));
    }

    /// <summary>
    /// <para>Draw a rectangle outline.</para>
    /// <para>绘制矩形描边。</para>
    /// </summary>
    public void DrawRectangle(RectF rect, Color color, float thickness = 1f)
    {
        var left = rect.Left;
        var top = rect.Top;
        var right = rect.Right;
        var bottom = rect.Bottom;

        DrawLine(new Vector2(left, top), new Vector2(right, top), color, thickness);
        DrawLine(new Vector2(right, top), new Vector2(right, bottom), color, thickness);
        DrawLine(new Vector2(right, bottom), new Vector2(left, bottom), color, thickness);
        DrawLine(new Vector2(left, bottom), new Vector2(left, top), color, thickness);
    }

    /// <summary>
    /// <para>Fill a rectangle.</para>
    /// <para>填充矩形。</para>
    /// </summary>
    public void FillRectangle(RectF rect, Color color)
    {
        if (!_begun || _whiteTexture == null || rect.Width <= 0 || rect.Height <= 0)
        {
            return;
        }

        var uv = new Vector2(0.5f, 0.5f);
        AddUiQuad(_whiteTexture,
            new UiQuad(
                new Vector2(rect.Left, rect.Top),
                new Vector2(rect.Right, rect.Top),
                new Vector2(rect.Right, rect.Bottom),
                new Vector2(rect.Left, rect.Bottom)),
            uv, uv, ToVector4(color));
    }

    private void EnsureInitialized()
    {
        if (_initialized)
        {
            return;
        }

        _initialized = true;

        _sampler = gpu.AddSampler(SDL.GPUFilter.Linear,
            SDL.GPUSamplerAddressMode.Repeat) ?? 0;
        _uiVertexBuffer = gpu.CreateBuffer(SDL.GPUBufferUsageFlags.Vertex,
            MaxVertices * MaxVertexStride) ?? 0;
        _materialVertexBuffer = gpu.CreateBuffer(SDL.GPUBufferUsageFlags.Vertex,
            MaxVertices * MaxVertexStride) ?? 0;
        _whiteTexture = CreateWhiteTexture();

        if (_sampler == 0 || _uiVertexBuffer == 0 || _materialVertexBuffer == 0 ||
            _whiteTexture == null)
        {
            LogCat.Error("Renderer2D: failed to create GPU resources.");
        }
    }

    private bool EnsureUiPipeline()
    {
        if (_uiPipelineHandle != 0)
        {
            return true;
        }

        if (_uiPipelineLoadFailed)
        {
            return false;
        }

        var pipelineRef = ResourceRef.Create(ResourceType.Pipeline, ResourceRef.CorePackId, "ui");
        var pipelineResult = appContext.AssetsPackManager.FindAsset<GraphicsPipeline>(appContext, pipelineRef);
        if (!pipelineResult.Exist || pipelineResult.Result == null)
        {
            _uiPipelineLoadFailed = true;
            LogCat.Debug("Renderer2D: the 'ui' pipeline is not available yet.");
            return false;
        }

        _uiPipeline = pipelineResult.Result;
        if (_uiPipeline.VertexLayout == null)
        {
            // Backward compatibility: pipelines without an explicit layout use the built-in UI format.
            // 向后兼容：未显式声明布局的管线使用内置 UI 格式。
            _uiPipeline.VertexLayout = VertexLayout.PositionTextureColor;
        }

        _uiLayout = _uiPipeline.VertexLayout;
        _uiPipelineHandle = gpu.AddGraphicsPipeline(_uiPipeline, _frameContext.SwapchainFormat) ?? 0;
        _uiPipelineLoadFailed = _uiPipelineHandle == 0;
        if (_uiPipelineLoadFailed)
        {
            LogCat.Error("Renderer2D: failed to create the UI graphics pipeline.");
        }

        return !_uiPipelineLoadFailed;
    }

    private void PrepareMaterialResources()
    {
        foreach (var command in _commands)
        {
            if (!command.IsMaterial || command.Material == null)
            {
                continue;
            }

            command.Pipeline = GetOrCreatePipeline(command.Material.Pipeline);
            command.Texture = GetOrCreateTexture(command.Material.Texture);
        }
    }

    private nint GetOrCreatePipeline(GraphicsPipeline? pipeline)
    {
        if (pipeline == null)
        {
            return 0;
        }

        if (_pipelineCache.TryGetValue(pipeline, out var existing))
        {
            return existing;
        }

        var handle = gpu.AddGraphicsPipeline(pipeline, _frameContext.SwapchainFormat) ?? 0;
        _pipelineCache[pipeline] = handle;
        return handle;
    }

    private Texture2D? GetOrCreateTexture(TextureData? data)
    {
        if (data == null)
        {
            return null;
        }

        if (_textureCache.TryGetValue(data, out var existing))
        {
            return existing;
        }

        var texture = new Texture2D(data);
        _textureCache[data] = texture;
        _pendingTextures.Add(texture);
        return texture;
    }

    private Texture2D CreateWhiteTexture()
    {
        var data = new TextureData
        {
            Width = 1,
            Height = 1,
            Pixels = [255, 255, 255, 255]
        };
        var texture = new Texture2D(data);
        _pendingTextures.Add(texture);
        return texture;
    }

    private void UploadPendingTextures()
    {
        foreach (var texture in _pendingTextures)
        {
            if (texture.Handle == 0)
            {
                texture.Handle = gpu.AddTexture(texture.Data, _frameContext.CommandBuffer) ?? 0;
            }
        }

        _pendingTextures.Clear();
    }

    private void UploadVertexBuffers()
    {
        if (_uiVertexCount > 0 && _uiVertexBuffer != 0 && _uiLayout != null)
        {
            var data = new ReadOnlySpan<byte>(_uiVertexData, 0, _uiVertexCount * _uiLayout.Stride);
            if (!gpu.UpdateBuffer(_uiVertexBuffer, data, _frameContext.CommandBuffer, true))
            {
                LogCat.Warn("Renderer2D: failed to upload the UI vertex buffer.");
            }
        }

        if (_materialVertexByteCount > 0 && _materialVertexBuffer != 0)
        {
            var data = new ReadOnlySpan<byte>(_materialVertexData, 0, _materialVertexByteCount);
            if (!gpu.UpdateBuffer(_materialVertexBuffer, data, _frameContext.CommandBuffer, true))
            {
                LogCat.Warn("Renderer2D: failed to upload the material vertex buffer.");
            }
        }
    }

    private void DrawCommands(IntPtr renderPass)
    {
        if (_sampler == 0)
        {
            return;
        }

        foreach (var command in _commands)
        {
            var texture = command.Texture;
            if (command.VertexCount <= 0 || texture == null || texture.Handle == 0)
            {
                continue;
            }

            if (!BindCommandPipeline(renderPass, command))
            {
                continue;
            }

            BindFragmentTexture(renderPass, texture.Handle);
            var firstVertex = command.IsMaterial ? 0u : (uint)command.FirstVertex;
            SDL.DrawGPUPrimitives(renderPass, (uint)command.VertexCount, 1, firstVertex, 0);
        }
    }

    private bool BindCommandPipeline(IntPtr renderPass, RenderCommand command)
    {
        if (command.IsMaterial)
        {
            return BindMaterialPipeline(renderPass, command);
        }

        return BindUiPipeline(renderPass);
    }

    private bool BindMaterialPipeline(IntPtr renderPass, RenderCommand command)
    {
        if (command.Pipeline == 0 || command.Material?.Pipeline == null ||
            _materialVertexBuffer == 0)
        {
            return false;
        }

        SDL.BindGPUGraphicsPipeline(renderPass, command.Pipeline);
        PushMaterialUniforms(command.Material, command.Tint);
        BindVertexBuffer(renderPass, _materialVertexBuffer, command.FirstVertexByteOffset);
        return true;
    }

    private bool BindUiPipeline(IntPtr renderPass)
    {
        if (_uiVertexBuffer == 0 || !EnsureUiPipeline())
        {
            return false;
        }

        SDL.BindGPUGraphicsPipeline(renderPass, _uiPipelineHandle);
        PushUiUniforms();
        BindVertexBuffer(renderPass, _uiVertexBuffer);
        return true;
    }

    private static void BindVertexBuffer(IntPtr renderPass, IntPtr vertexBuffer, int offsetBytes = 0)
    {
        var binding = new SDL.GPUBufferBinding
        {
            Buffer = vertexBuffer,
            Offset = (uint)offsetBytes
        };
        SDL.BindGPUVertexBuffers(renderPass, 0, [binding], 1);
    }

    private void BindFragmentTexture(IntPtr renderPass, IntPtr texture)
    {
        var binding = new SDL.GPUTextureSamplerBinding
        {
            Texture = texture,
            Sampler = _sampler
        };
        SDL.BindGPUFragmentSamplers(renderPass, 0, [binding], 1);
    }

    private void PushMaterialUniforms(Material material, Color? tint)
    {
        if (material.Pipeline == null)
        {
            return;
        }

        var previousTint = RuntimeUniformState.TintColor;
        if (tint.HasValue)
        {
            RuntimeUniformState.TintColor = ToVector4(tint.Value);
        }

        try
        {
            for (var i = 0; i < material.Pipeline.VertexUniformBlocks.Count; i++)
            {
                var data = material.Pipeline.VertexUniformBlocks[i]
                    .BuildData(appContext.RuntimeUniformManager);
                SDL.PushGPUVertexUniformData(_frameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
            }

            for (var i = 0; i < material.Pipeline.FragmentUniformBlocks.Count; i++)
            {
                var data = material.Pipeline.FragmentUniformBlocks[i]
                    .BuildData(appContext.RuntimeUniformManager);
                SDL.PushGPUFragmentUniformData(_frameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
            }
        }
        finally
        {
            RuntimeUniformState.TintColor = previousTint;
        }
    }

    private void PushUiUniforms()
    {
        if (_uiPipeline == null)
        {
            return;
        }

        for (var i = 0; i < _uiPipeline.VertexUniformBlocks.Count; i++)
        {
            var data = _uiPipeline.VertexUniformBlocks[i].BuildData(appContext.RuntimeUniformManager);
            SDL.PushGPUVertexUniformData(_frameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
        }

        for (var i = 0; i < _uiPipeline.FragmentUniformBlocks.Count; i++)
        {
            var data = _uiPipeline.FragmentUniformBlocks[i].BuildData(appContext.RuntimeUniformManager);
            SDL.PushGPUFragmentUniformData(_frameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
        }
    }

    private void AddUiQuad(Texture2D texture, UiQuad quad, Vector2 uvMin, Vector2 uvMax,
        Vector4 color)
    {
        if (_uiLayout == null && !EnsureUiPipeline())
        {
            return;
        }

        if (_uiLayout == null)
        {
            return;
        }

        if (_uiVertexCount + 6 > MaxVertices)
        {
            LogCat.Warn("Renderer2D: the UI vertex buffer is full; further geometry was skipped.");
            return;
        }

        EnsurePrimitiveCommand(texture);

        AddUiVertex(quad.TopLeft, uvMin, color);
        AddUiVertex(quad.TopRight, new Vector2(uvMax.X, uvMin.Y), color);
        AddUiVertex(quad.BottomRight, uvMax, color);
        AddUiVertex(quad.TopLeft, uvMin, color);
        AddUiVertex(quad.BottomRight, uvMax, color);
        AddUiVertex(quad.BottomLeft, new Vector2(uvMin.X, uvMax.Y), color);

        _currentPrimitiveCommand!.VertexCount += 6;
    }

    private readonly record struct UiQuad(Vector2 TopLeft, Vector2 TopRight, Vector2 BottomRight,
        Vector2 BottomLeft);

    private void AddMaterialQuad(Vector2 topLeft, Vector2 topRight, Vector2 bottomRight,
        Vector2 bottomLeft, Vector2 uvMin, Vector2 uvMax, VertexLayout layout)
    {
        AddMaterialVertex(topLeft, uvMin, layout);
        AddMaterialVertex(topRight, new Vector2(uvMax.X, uvMin.Y), layout);
        AddMaterialVertex(bottomRight, uvMax, layout);
        AddMaterialVertex(topLeft, uvMin, layout);
        AddMaterialVertex(bottomRight, uvMax, layout);
        AddMaterialVertex(bottomLeft, new Vector2(uvMin.X, uvMax.Y), layout);
    }

    private void EnsurePrimitiveCommand(Texture2D texture)
    {
        if (_currentPrimitiveCommand != null &&
            ReferenceEquals(_currentPrimitiveCommand.Texture, texture))
        {
            return;
        }

        _currentPrimitiveCommand = new RenderCommand
        {
            IsMaterial = false,
            Texture = texture,
            FirstVertex = _uiVertexCount
        };
        _commands.Add(_currentPrimitiveCommand);
    }

    private void AddUiVertex(Vector2 position, Vector2 uv, Vector4 color)
    {
        var layout = _uiLayout;
        if (layout == null)
        {
            return;
        }

        WriteVertex(_uiVertexData, _uiVertexCount * layout.Stride, layout, position, uv, color);
        _uiVertexCount++;
    }

    private void AddMaterialVertex(Vector2 position, Vector2 uv, VertexLayout layout)
    {
        WriteVertex(_materialVertexData, _materialVertexByteCount, layout, position, uv,
            Vector4.One);
        _materialVertexCount++;
        _materialVertexByteCount += layout.Stride;
    }

    private static void WriteVertex(byte[] destination, int vertexOffset, VertexLayout layout,
        Vector2 position, Vector2 uv, Vector4 color)
    {
        foreach (var attribute in layout.Attributes)
        {
            var offset = vertexOffset + attribute.Offset;
            switch (attribute.Semantic)
            {
                case VertexSemantics.Position:
                    WriteFloat2(destination, offset, position);
                    break;
                case VertexSemantics.Uv:
                    WriteFloat2(destination, offset, uv);
                    break;
                case VertexSemantics.Color:
                    WriteFloat4(destination, offset, color);
                    break;
            }
        }
    }

    private static void WriteFloat2(byte[] destination, int offset, Vector2 value)
    {
        BinaryPrimitives.WriteSingleLittleEndian(destination.AsSpan(offset, sizeof(float)), value.X);
        BinaryPrimitives.WriteSingleLittleEndian(destination.AsSpan(offset + sizeof(float), sizeof(float)),
            value.Y);
    }

    private static void WriteFloat4(byte[] destination, int offset, Vector4 value)
    {
        BinaryPrimitives.WriteSingleLittleEndian(destination.AsSpan(offset, sizeof(float)), value.X);
        BinaryPrimitives.WriteSingleLittleEndian(destination.AsSpan(offset + sizeof(float), sizeof(float)),
            value.Y);
        BinaryPrimitives.WriteSingleLittleEndian(
            destination.AsSpan(offset + 2 * sizeof(float), sizeof(float)), value.Z);
        BinaryPrimitives.WriteSingleLittleEndian(
            destination.AsSpan(offset + 3 * sizeof(float), sizeof(float)), value.W);
    }

    private void ResetFrame()
    {
        _uiVertexCount = 0;
        _materialVertexCount = 0;
        _materialVertexByteCount = 0;
        _commands.Clear();
        _currentPrimitiveCommand = null;
    }

    private static SDL.FColor ToFColor(Color color)
    {
        return new SDL.FColor
        {
            R = color.R / 255f,
            G = color.G / 255f,
            B = color.B / 255f,
            A = color.A / 255f
        };
    }

    private static Vector4 ToVector4(Color color)
    {
        return new Vector4(color.R / 255f, color.G / 255f, color.B / 255f, color.A / 255f);
    }
}