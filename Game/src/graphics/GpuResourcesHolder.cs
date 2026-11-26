using GlimmerWorks.Engine.Log;
using SDL3;

namespace GlimmerWorks.Game.Graphics;

public class GpuResourcesHolder
{
    /// <summary>
    /// <para>GpuDevice</para>
    /// <para>GPU设备</para>
    /// </summary>
    public nint GpuDevice { get; set; }

    private readonly List<nint> _shaderList = new();
    private readonly List<nint> _textureList = new();
    private readonly List<nint> _samplerList = new();
    private readonly List<nint> _bufferList = new();
    private readonly List<nint> _pipelineList = new();

    /// <summary>
    /// <para>AddShader</para>
    /// <para>添加着色器</para>
    /// </summary>
    /// <param name="vertexShader"></param>
    /// <param name="compiledShader"></param>
    /// <returns></returns>
    public nint? AddShader(bool vertexShader, CompiledShader compiledShader)
    {
        if (GpuDevice == IntPtr.Zero)
        {
            LogCat.Error("Failed to create GPU shader: GPU device is not available.");
            return null;
        }

        if (compiledShader.Spirv is not { Length: > 0 })
        {
            LogCat.Error("Failed to create GPU shader: SPIR-V bytecode is empty.");
            return null;
        }

        var createInfo = new SDL.GPUShaderCreateInfo
        {
            Stage = vertexShader ? SDL.GPUShaderStage.Vertex : SDL.GPUShaderStage.Fragment,
            Format = SDL.GPUShaderFormat.SPIRV,
            NumSamplers = compiledShader.NumSamplers,
            NumUniformBuffers = compiledShader.NumUniformBuffers,
            NumStorageTextures = compiledShader.NumStorageTextures,
            NumStorageBuffers = compiledShader.NumStorageBuffers
        };

        var shader = SDL.CreateGPUShader(
            GpuDevice,
            in createInfo,
            compiledShader.Spirv,
            ShaderCompiler.DefaultEntryPoint);

        if (shader == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU shader: {SDL.GetError()}");
            return null;
        }

        _shaderList.Add(shader);
        return shader;
    }

    /// <summary>
    /// <para>AddTexture</para>
    /// <para>把解码后的纹理上传到 GPU。</para>
    /// </summary>
    /// <param name="texture">
    ///<para>The decoded texture.</para>
    ///<para>解码后的纹理。</para>
    /// </param>
    /// <param name="commandBuffer">
    ///<para>The command buffer used for the upload.</para>
    ///<para>用于上传的命令缓冲区。</para>
    /// </param>
    /// <returns></returns>
    public nint? AddTexture(TextureData texture, nint commandBuffer)
    {
        if (GpuDevice == IntPtr.Zero || commandBuffer == IntPtr.Zero)
        {
            LogCat.Error("Failed to create GPU texture: GPU device or command buffer is not available.");
            return null;
        }

        if (texture is not { Width: > 0, Height: > 0 } || texture.Pixels.Length < texture.Width * texture.Height * 4)
        {
            LogCat.Error("Failed to create GPU texture: the texture data is invalid.");
            return null;
        }

        var createInfo = new SDL.GPUTextureCreateInfo
        {
            Type = SDL.GPUTextureType.TextureType2D,
            Format = SDL.GPUTextureFormat.R8G8B8A8Unorm,
            Usage = SDL.GPUTextureUsageFlags.Sampler,
            Width = (uint)texture.Width,
            Height = (uint)texture.Height,
            LayerCountOrDepth = 1,
            NumLevels = 1,
            SampleCount = SDL.GPUSampleCount.SampleCount1
        };

        var gpuTexture = SDL.CreateGPUTexture(GpuDevice, in createInfo);
        if (gpuTexture == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU texture: {SDL.GetError()}");
            return null;
        }

        var transferBuffer = UploadData(texture.Pixels, out _);
        if (transferBuffer == IntPtr.Zero)
        {
            SDL.ReleaseGPUTexture(GpuDevice, gpuTexture);
            return null;
        }

        try
        {
            var copyPass = SDL.BeginGPUCopyPass(commandBuffer);
            var source = new SDL.GPUTextureTransferInfo
            {
                TransferBuffer = transferBuffer,
                Offset = 0,
                PixelsPerRow = (uint)texture.Width,
                RowsPerLayer = (uint)texture.Height
            };
            var destination = new SDL.GPUTextureRegion
            {
                Texture = gpuTexture,
                MipLevel = 0,
                Layer = 0,
                X = 0,
                Y = 0,
                Z = 0,
                W = (uint)texture.Width,
                H = (uint)texture.Height,
                D = 1
            };
            SDL.UploadToGPUTexture(copyPass, in source, in destination, false);
            SDL.EndGPUCopyPass(copyPass);
        }
        finally
        {
            SDL.ReleaseGPUTransferBuffer(GpuDevice, transferBuffer);
        }

        _textureList.Add(gpuTexture);
        return gpuTexture;
    }

    /// <summary>
    /// <para>AddSampler</para>
    /// <para>创建采样器。</para>
    /// </summary>
    /// <param name="filter">
    ///<para>The min/mag filter.</para>
    ///<para>缩小/放大过滤方式。</para>
    /// </param>
    /// <param name="addressMode">
    ///<para>The address mode.</para>
    ///<para>寻址模式。</para>
    /// </param>
    /// <returns></returns>
    public nint? AddSampler(SDL.GPUFilter filter = SDL.GPUFilter.Linear,
        SDL.GPUSamplerAddressMode addressMode = SDL.GPUSamplerAddressMode.ClampToEdge)
    {
        if (GpuDevice == IntPtr.Zero)
        {
            LogCat.Error("Failed to create GPU sampler: GPU device is not available.");
            return null;
        }

        var createInfo = new SDL.GPUSamplerCreateInfo
        {
            MinFilter = filter,
            MagFilter = filter,
            MipmapMode = SDL.GPUSamplerMipmapMode.Nearest,
            AddressModeU = addressMode,
            AddressModeV = addressMode,
            AddressModeW = addressMode,
            CompareOp = SDL.GPUCompareOp.Invalid,
            MinLod = 0,
            MaxLod = 0
        };

        var sampler = SDL.CreateGPUSampler(GpuDevice, in createInfo);
        if (sampler == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU sampler: {SDL.GetError()}");
            return null;
        }

        _samplerList.Add(sampler);
        return sampler;
    }

    /// <summary>
    /// <para>AddBuffer</para>
    /// <para>把数据上传到 GPU 缓冲区。</para>
    /// </summary>
    /// <param name="usage"></param>
    /// <param name="data"></param>
    /// <param name="commandBuffer"></param>
    /// <returns></returns>
    public nint? AddBuffer(SDL.GPUBufferUsageFlags usage, ReadOnlySpan<byte> data, nint commandBuffer)
    {
        if (GpuDevice == IntPtr.Zero || commandBuffer == IntPtr.Zero)
        {
            LogCat.Error("Failed to create GPU buffer: GPU device or command buffer is not available.");
            return null;
        }

        if (data.Length == 0)
        {
            LogCat.Error("Failed to create GPU buffer: the data is empty.");
            return null;
        }

        var createInfo = new SDL.GPUBufferCreateInfo
        {
            Usage = usage,
            Size = (uint)data.Length
        };
        var buffer = SDL.CreateGPUBuffer(GpuDevice, in createInfo);
        if (buffer == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU buffer: {SDL.GetError()}");
            return null;
        }

        var transferBuffer = UploadData(data, out _);
        if (transferBuffer == IntPtr.Zero)
        {
            SDL.ReleaseGPUBuffer(GpuDevice, buffer);
            return null;
        }

        try
        {
            var copyPass = SDL.BeginGPUCopyPass(commandBuffer);
            var source = new SDL.GPUTransferBufferLocation
            {
                TransferBuffer = transferBuffer,
                Offset = 0
            };
            var destination = new SDL.GPUBufferRegion
            {
                Buffer = buffer,
                Offset = 0,
                Size = (uint)data.Length
            };
            SDL.UploadToGPUBuffer(copyPass, in source, in destination, false);
            SDL.EndGPUCopyPass(copyPass);
        }
        finally
        {
            SDL.ReleaseGPUTransferBuffer(GpuDevice, transferBuffer);
        }

        _bufferList.Add(buffer);
        return buffer;
    }

    /// <summary>
    /// <para>AddGraphicsPipeline</para>
    /// <para>根据材质管线创建图形管线。</para>
    /// </summary>
    /// <param name="pipeline">
    ///<para>The resolved material pipeline.</para>
    ///<para>解析后的材质管线。</para>
    /// </param>
    /// <param name="targetFormat">
    ///<para>The swapchain color target format.</para>
    ///<para>交换链颜色目标格式。</para>
    /// </param>
    /// <returns></returns>
    public nint? AddGraphicsPipeline(Pipeline pipeline, SDL.GPUTextureFormat targetFormat)
    {
        if (GpuDevice == IntPtr.Zero)
        {
            LogCat.Error("Failed to create GPU graphics pipeline: GPU device is not available.");
            return null;
        }

        if (pipeline.VertexShader == null || pipeline.FragmentShader == null)
        {
            LogCat.Error("Failed to create GPU graphics pipeline: the shaders are missing.");
            return null;
        }

        var vertexShader = AddShader(true, pipeline.VertexShader);
        var fragmentShader = AddShader(false, pipeline.FragmentShader);
        if (vertexShader == null || fragmentShader == null)
        {
            return null;
        }

        var vertexBuffers = new[]
        {
            new SDL.GPUVertexBufferDescription
            {
                Slot = 0,
                Pitch = VertexStride,
                InputRate = SDL.GPUVertexInputRate.Vertex,
                InstanceStepRate = 0
            }
        };
        var vertexAttributes = new[]
        {
            new SDL.GPUVertexAttribute
            {
                Location = 0,
                BufferSlot = 0,
                Format = SDL.GPUVertexElementFormat.Float2,
                Offset = 0
            },
            new SDL.GPUVertexAttribute
            {
                Location = 1,
                BufferSlot = 0,
                Format = SDL.GPUVertexElementFormat.Float2,
                Offset = 8
            }
        };
        var colorTargets = new[]
        {
            new SDL.GPUColorTargetDescription
            {
                Format = targetFormat,
                BlendState = GetBlendState(pipeline.BlendMode)
            }
        };

        var createInfo = new SDL.GPUGraphicsPipelineCreateInfo
        {
            VertexShader = vertexShader.Value,
            FragmentShader = fragmentShader.Value,
            PrimitiveType = SDL.GPUPrimitiveType.TriangleList,
            VertexInputState = new SDL.GPUVertexInputState
            {
                NumVertexBuffers = (uint)vertexBuffers.Length,
                NumVertexAttributes = (uint)vertexAttributes.Length
            },
            RasterizerState = new SDL.GPURasterizerState
            {
                FillMode = SDL.GPUFillMode.Fill,
                CullMode = SDL.GPUCullMode.None,
                FrontFace = SDL.GPUFrontFace.CounterClockwise
            },
            MultisampleState = new SDL.GPUMultisampleState
            {
                SampleCount = SDL.GPUSampleCount.SampleCount1,
                SampleMask = 0
            },
            DepthStencilState = new SDL.GPUDepthStencilState
            {
                CompareOp = SDL.GPUCompareOp.Invalid
            },
            TargetInfo = new SDL.GPUGraphicsPipelineTargetInfo
            {
                NumColorTargets = (uint)colorTargets.Length
            }
        };

        var graphicsPipeline = SDL.CreateGPUGraphicsPipeline(GpuDevice, in createInfo,
            vertexBuffers, vertexAttributes, colorTargets);
        if (graphicsPipeline == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU graphics pipeline: {SDL.GetError()}");
            return null;
        }

        _pipelineList.Add(graphicsPipeline);
        return graphicsPipeline;
    }

    /// <summary>
    /// <para>The byte stride of the engine's default vertex format (position + UV).</para>
    /// <para>引擎默认顶点格式的字节步长（位置 + UV）。</para>
    /// </summary>
    public const uint VertexStride = 4 * sizeof(float);

    private static SDL.GPUColorTargetBlendState GetBlendState(BlendMode blendMode)
    {
        var writeMask = SDL.GPUColorComponentFlags.R | SDL.GPUColorComponentFlags.G |
                        SDL.GPUColorComponentFlags.B | SDL.GPUColorComponentFlags.A;
        return blendMode switch
        {
            BlendMode.Alpha => new SDL.GPUColorTargetBlendState
            {
                SrcColorBlendFactor = SDL.GPUBlendFactor.SrcAlpha,
                DstColorBlendFactor = SDL.GPUBlendFactor.OneMinusSrcAlpha,
                ColorBlendOp = SDL.GPUBlendOp.Add,
                SrcAlphaBlendFactor = SDL.GPUBlendFactor.SrcAlpha,
                DstAlphaBlendFactor = SDL.GPUBlendFactor.OneMinusSrcAlpha,
                AlphaBlendOp = SDL.GPUBlendOp.Add,
                ColorWriteMask = writeMask
            },
            BlendMode.Additive => new SDL.GPUColorTargetBlendState
            {
                SrcColorBlendFactor = SDL.GPUBlendFactor.SrcAlpha,
                DstColorBlendFactor = SDL.GPUBlendFactor.One,
                ColorBlendOp = SDL.GPUBlendOp.Add,
                SrcAlphaBlendFactor = SDL.GPUBlendFactor.SrcAlpha,
                DstAlphaBlendFactor = SDL.GPUBlendFactor.One,
                AlphaBlendOp = SDL.GPUBlendOp.Add,
                ColorWriteMask = writeMask
            },
            _ => new SDL.GPUColorTargetBlendState
            {
                SrcColorBlendFactor = SDL.GPUBlendFactor.One,
                DstColorBlendFactor = SDL.GPUBlendFactor.Zero,
                ColorBlendOp = SDL.GPUBlendOp.Add,
                SrcAlphaBlendFactor = SDL.GPUBlendFactor.One,
                DstAlphaBlendFactor = SDL.GPUBlendFactor.Zero,
                AlphaBlendOp = SDL.GPUBlendOp.Add,
                ColorWriteMask = writeMask
            }
        };
    }

    private nint UploadData(ReadOnlySpan<byte> data, out uint size)
    {
        size = (uint)data.Length;
        var createInfo = new SDL.GPUTransferBufferCreateInfo
        {
            Usage = SDL.GPUTransferBufferUsage.Upload,
            Size = size
        };
        var transferBuffer = SDL.CreateGPUTransferBuffer(GpuDevice, in createInfo);
        if (transferBuffer == IntPtr.Zero)
        {
            LogCat.Error($"Failed to create GPU transfer buffer: {SDL.GetError()}");
            return IntPtr.Zero;
        }

        var mapped = SDL.MapGPUTransferBuffer(GpuDevice, transferBuffer, false);
        if (mapped == IntPtr.Zero)
        {
            LogCat.Error($"Failed to map GPU transfer buffer: {SDL.GetError()}");
            SDL.ReleaseGPUTransferBuffer(GpuDevice, transferBuffer);
            return IntPtr.Zero;
        }

        unsafe
        {
            fixed (byte* source = data)
            {
                Buffer.MemoryCopy(source, (void*)mapped, size, size);
            }
        }

        SDL.UnmapGPUTransferBuffer(GpuDevice, transferBuffer);
        return transferBuffer;
    }

    /// <summary>
    /// <para>RemoveShader</para>
    /// <para>移除着色器</para>
    /// </summary>
    /// <param name="shader"></param>
    public void RemoveShader(nint shader)
    {
        if (GpuDevice == IntPtr.Zero || shader == IntPtr.Zero)
        {
            return;
        }

        if (!_shaderList.Remove(shader))
        {
            return;
        }

        SDL.ReleaseGPUShader(GpuDevice, shader);
    }

    public void Destroy()
    {
        if (GpuDevice == IntPtr.Zero)
        {
            _shaderList.Clear();
            _textureList.Clear();
            _samplerList.Clear();
            _bufferList.Clear();
            _pipelineList.Clear();
            return;
        }

        foreach (var pipeline in _pipelineList)
        {
            if (pipeline != IntPtr.Zero)
            {
                SDL.ReleaseGPUGraphicsPipeline(GpuDevice, pipeline);
            }
        }

        _pipelineList.Clear();

        foreach (var shader in _shaderList)
        {
            if (shader != IntPtr.Zero)
            {
                SDL.ReleaseGPUShader(GpuDevice, shader);
            }
        }

        _shaderList.Clear();

        foreach (var buffer in _bufferList)
        {
            if (buffer != IntPtr.Zero)
            {
                SDL.ReleaseGPUBuffer(GpuDevice, buffer);
            }
        }

        _bufferList.Clear();

        foreach (var sampler in _samplerList)
        {
            if (sampler != IntPtr.Zero)
            {
                SDL.ReleaseGPUSampler(GpuDevice, sampler);
            }
        }

        _samplerList.Clear();

        foreach (var texture in _textureList)
        {
            if (texture != IntPtr.Zero)
            {
                SDL.ReleaseGPUTexture(GpuDevice, texture);
            }
        }

        _textureList.Clear();
    }
}
