using System.Numerics;
using System.Runtime.InteropServices;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Game.Graphics;
using GlimmerWorks.Game.Graphics.Uniform;
using SDL3;

namespace GlimmerWorks.Game.Scene;

public class TestScene(GameAppContext appContext) : BaseScene(appContext)
{
    private Material? _material;
    private nint _graphicsPipeline;
    private nint _texture;
    private nint _sampler;
    private nint _vertexBuffer;
    private nint _indexBuffer;
    private long _tick;
    private bool _renderLogged;

    protected override void EnsureGpuResources(in SceneFrameContext sceneFrameContext)
    {
        base.EnsureGpuResources(in sceneFrameContext);
        GpuResourcesHolder.GpuDevice = sceneFrameContext.Device;

        var materialResult = AppContext.AssetsPackManager.FindAsset<Material>(AppContext,
            ResourceRef.Create(ResourceType.Material, ResourceRef.CorePackId, ResourceRef.CorePackId, "test")
        );
        if (!materialResult.Exist || materialResult.Result is not { Pipeline: not null, Texture: not null })
        {
            LogCat.Error("TestScene: failed to load the material 'test'.");
            return;
        }

        _material = materialResult.Result;
        _graphicsPipeline =
            GpuResourcesHolder.AddGraphicsPipeline(_material.Pipeline, sceneFrameContext.SwapchainFormat) ?? 0;
        _texture = GpuResourcesHolder.AddTexture(_material.Texture, sceneFrameContext.CommandBuffer) ?? 0;
        _sampler = GpuResourcesHolder.AddSampler(SDL.GPUFilter.Linear) ?? 0;

        // A quad in pixel coordinates, with UVs. The vertex layout is
        // position (vec2) + uv (vec2), matching the pipeline's vertex input.
        // 像素坐标的四边形与 UV。顶点布局为 position(vec2) + uv(vec2)，
        // 与管线的顶点输入一致。
        const float displayWidth = 512f;
        var displayHeight = displayWidth * _material.Texture.Height / _material.Texture.Width;
        const float left = 160f;
        const float top = 120f;
        var right = left + displayWidth;
        var bottom = top + displayHeight;

        var vertices = new[]
        {
            left, top, 0f, 0f,
            right, top, 1f, 0f,
            right, bottom, 1f, 1f,
            left, bottom, 0f, 1f
        };
        var indices = new ushort[] { 0, 1, 2, 0, 2, 3 };

        _vertexBuffer = GpuResourcesHolder.AddBuffer(SDL.GPUBufferUsageFlags.Vertex,
            MemoryMarshal.AsBytes(vertices.AsSpan()), sceneFrameContext.CommandBuffer) ?? 0;
        _indexBuffer = GpuResourcesHolder.AddBuffer(SDL.GPUBufferUsageFlags.Index,
            MemoryMarshal.AsBytes(indices.AsSpan()), sceneFrameContext.CommandBuffer) ?? 0;

        if (_graphicsPipeline == 0 || _texture == 0 || _sampler == 0 ||
            _vertexBuffer == 0 || _indexBuffer == 0)
        {
            LogCat.Error("TestScene: failed to create GPU resources.");
            return;
        }

        LogCat.Debug(
            $"TestScene: material loaded ({_material.Texture.Width}x{_material.Texture.Height}), " +
            $"pipeline={_graphicsPipeline}, texture={_texture}, sampler={_sampler}, " +
            $"vertexBuffer={_vertexBuffer}, indexBuffer={_indexBuffer}");
    }

    public override void OnTick(long deltaMs, long globalTick)
    {
        _tick = globalTick;
    }

    public override void RenderFrame(in SceneFrameContext sceneFrameContext)
    {
        if (_graphicsPipeline == 0 || _material?.Pipeline == null)
        {
            return;
        }

        RuntimeUniformState.ViewSize = new Vector2(sceneFrameContext.Width, sceneFrameContext.Height);
        RuntimeUniformState.Tick = _tick;

        if (!_renderLogged)
        {
            _renderLogged = true;
            LogCat.Debug(
                $"TestScene: first frame {sceneFrameContext.Width}x{sceneFrameContext.Height}, " +
                $"swapchain={sceneFrameContext.SwapchainTexture}, format={sceneFrameContext.SwapchainFormat}");
        }

        ReadOnlySpan<SDL.GPUColorTargetInfo> colorTargets =
        [
            new()
            {
                Texture = sceneFrameContext.SwapchainTexture,
                LoadOp = SDL.GPULoadOp.Clear,
                StoreOp = SDL.GPUStoreOp.Store,
                ClearColor = new SDL.FColor { R = 0.05f, G = 0.05f, B = 0.08f, A = 1f }
            }
        ];

        var renderPass = SDL.BeginGPURenderPass(sceneFrameContext.CommandBuffer, colorTargets, 1, IntPtr.Zero);
        if (renderPass == IntPtr.Zero)
        {
            LogCat.Error($"Failed to begin a render pass: {SDL.GetError()}");
            return;
        }

        try
        {
            SDL.BindGPUGraphicsPipeline(renderPass, _graphicsPipeline);

            var pipeline = _material.Pipeline;
            for (var i = 0; i < pipeline.VertexUniformBlocks.Count; i++)
            {
                var data = pipeline.VertexUniformBlocks[i].BuildData(AppContext.RuntimeUniformManager);
                SDL.PushGPUVertexUniformData(sceneFrameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
            }

            for (var i = 0; i < pipeline.FragmentUniformBlocks.Count; i++)
            {
                var data = pipeline.FragmentUniformBlocks[i].BuildData(AppContext.RuntimeUniformManager);
                SDL.PushGPUFragmentUniformData(sceneFrameContext.CommandBuffer, (uint)i, data, (uint)data.Length);
            }

            SDL.BindGPUVertexBuffers(renderPass, 0,
                [new SDL.GPUBufferBinding { Buffer = _vertexBuffer, Offset = 0 }], 1);
            SDL.BindGPUIndexBuffer(renderPass,
                new SDL.GPUBufferBinding { Buffer = _indexBuffer, Offset = 0 },
                SDL.GPUIndexElementSize.IndexElementSize16Bit);
            SDL.BindGPUFragmentSamplers(renderPass, 0,
                [new SDL.GPUTextureSamplerBinding { Texture = _texture, Sampler = _sampler }], 1);

            SDL.DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
        }
        finally
        {
            SDL.EndGPURenderPass(renderPass);
        }
    }

    public override void OnDestroy()
    {
        GpuResourcesHolder.Destroy();
        base.OnDestroy();
    }
}
