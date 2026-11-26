using System.Buffers.Binary;
using System.Runtime.InteropServices;
using System.Text;
using GlimmerWorks.Engine.Log;
using Silk.NET.Shaderc;
using ShadercApi = Silk.NET.Shaderc.Shaderc;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>Compiles GLSL shaders into SPIR-V at runtime.</para>
/// <para>在运行时把 GLSL 着色器编译为 SPIR-V</para>
/// </summary>
/// <remarks>
/// <para>
/// The caller supplies the shader stage, so the GLSL source does not need a
/// <c>#pragma shader_stage(...)</c> directive.
/// </para>
/// <para>
/// 着色器阶段由调用方提供，因此 GLSL 源码不需要
/// <c>#pragma shader_stage(...)</c> 指令。
/// </para>
/// </remarks>
internal static class ShaderCompiler
{
    /// <summary>
    /// <para>The default shader entry point.</para>
    /// <para>默认着色器入口函数名</para>
    /// </summary>
    public const string DefaultEntryPoint = "main";

    /// <summary>
    /// <para>The magic number at the beginning of every SPIR-V module.</para>
    /// <para>每个 SPIR-V 模块开头的魔数。</para>
    /// </summary>
    private const uint SpirvMagic = 0x07230203;

    /// <summary>
    /// <para>The size of the SPIR-V header: five 32-bit words.</para>
    /// <para>SPIR-V 头部大小：5 个 32 位字。</para>
    /// </summary>
    private const int SpirvHeaderSize = 5 * sizeof(uint);

    // SPIR-V opcodes used by resource reflection.
    // 用于资源反射的 SPIR-V 操作码。
    private const int OpTypeImage = 25;
    private const int OpTypeSampler = 26;
    private const int OpTypeSampledImage = 27;
    private const int OpTypeArray = 28;
    private const int OpTypeRuntimeArray = 29;
    private const int OpTypeStruct = 30;
    private const int OpTypePointer = 32;
    private const int OpConstant = 43;
    private const int OpVariable = 59;
    private const int OpDecorate = 71;

    private const uint DecorationBufferBlock = 3;

    private const uint StorageClassUniformConstant = 0;
    private const uint StorageClassUniform = 2;
    private const uint StorageClassStorageBuffer = 12;

    /// <summary>
    /// <para>
    /// The lazily loaded shaderc API. Deferring the load means a missing native
    /// library only breaks shader compilation instead of the whole application.
    /// </para>
    /// <para>
    /// 延迟加载的 shaderc API。延迟加载可以让原生库缺失只影响着色器编译，
    /// 而不会导致整个应用启动失败。
    /// </para>
    /// </summary>
    private static readonly Lazy<ShadercApi> Api = new(ShadercApi.GetApi);

    /// <summary>
    /// <para>Compiles GLSL source into SPIR-V for the requested shader stage.</para>
    /// <para>把 GLSL 源码编译为指定阶段的 SPIR-V。</para>
    /// </summary>
    /// <param name="vertexShader">
    /// <para><c>true</c> to compile as a vertex shader; <c>false</c> for a fragment shader.</para>
    /// <para><c>true</c> 表示编译为顶点着色器；<c>false</c> 表示编译为片元着色器。</para>
    /// </param>
    /// <param name="fileName">
    /// <para>
    /// The shader file name used in compiler diagnostics. When it is empty,
    /// <c>&lt;memory&gt;</c> is used instead.
    /// </para>
    /// <para>
    /// 用于编译错误信息的着色器文件名。为空时使用 <c>&lt;memory&gt;</c>。
    /// </para>
    /// </param>
    /// <param name="bytes">
    /// <para>The GLSL source code encoded with UTF-8.</para>
    /// <para>UTF-8 编码的 GLSL 源码。</para>
    /// </param>
    /// <returns>
    /// <para>The compiled shader, or <c>null</c> if compilation failed.</para>
    /// <para>编译结果；失败时返回 <c>null</c>。</para>
    /// </returns>
    public static CompiledShader? Compile(bool vertexShader, string fileName, byte[] bytes)
    {
        // Empty or missing source cannot produce a shader.
        // 空源码无法生成着色器，直接失败。
        if (bytes is not { Length: > 0 })
        {
            LogCat.Error("Failed to compile shader: the source is empty.");
            return null;
        }

        // Windows editors may prepend a UTF-8 BOM, which is not valid GLSL.
        // Windows 编辑器可能写入 UTF-8 BOM，它不是合法的 GLSL 内容。
        var source = RemoveUtf8Bom(bytes);
        var shaderKind = vertexShader ? ShaderKind.GlslVertexShader : ShaderKind.GlslFragmentShader;
        var spirv = CompileGlsl(source, shaderKind, fileName);
        if (spirv == null)
        {
            return null;
        }

        // Reflect the resource declarations so SDL can create the matching shader.
        // 反射资源声明，供 SDL 创建匹配的着色器使用。
        var resourceCounts = ReadResourceCounts(spirv);

        return new CompiledShader
        {
            Spirv = spirv,
            VertexShader = vertexShader,
            NumSamplers = resourceCounts.NumSamplers,
            NumUniformBuffers = resourceCounts.NumUniformBuffers,
            NumStorageTextures = resourceCounts.NumStorageTextures,
            NumStorageBuffers = resourceCounts.NumStorageBuffers
        };
    }

    /// <summary>
    /// <para>Removes the UTF-8 byte order mark (BOM) at the beginning of the source.</para>
    /// <para>移除源码开头的 UTF-8 BOM（EF BB BF），避免它被当作 GLSL 内容导致编译失败。</para>
    /// </summary>
    /// <param name="bytes">
    /// <para>The original shader source bytes.</para>
    /// <para>原始的着色器源码字节。</para>
    /// </param>
    /// <returns>
    /// <para>The source bytes without a BOM, or the original array when no BOM is present.</para>
    /// <para>去掉 BOM 后的源码字节；如果没有 BOM，则返回原数组。</para>
    /// </returns>
    private static byte[] RemoveUtf8Bom(byte[] bytes)
    {
        // UTF-8 BOM = EF BB BF. Return a slice without copying when it is absent.
        // UTF-8 BOM 为 EF BB BF；不存在时直接返回原数组，避免多余分配。
        return bytes.Length >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF
            ? bytes[3..]
            : bytes;
    }

    /// <summary>
    /// <para>Compiles GLSL source into SPIR-V bytes with shaderc.</para>
    /// <para>使用 shaderc 把 GLSL 源码编译为 SPIR-V 字节码。</para>
    /// </summary>
    /// <param name="source">
    /// <para>The GLSL source code encoded with UTF-8.</para>
    /// <para>UTF-8 编码的 GLSL 源码。</para>
    /// </param>
    /// <param name="shaderKind">
    /// <para>The shaderc shader kind that determines the compile stage.</para>
    /// <para>决定编译阶段的 shaderc 着色器类型。</para>
    /// </param>
    /// <param name="fileName">
    /// <para>The shader file name used in compiler diagnostics.</para>
    /// <para>用于编译错误信息的着色器文件名。</para>
    /// </param>
    /// <returns>
    /// <para>The SPIR-V bytecode, or <c>null</c> when compilation fails.</para>
    /// <para>SPIR-V 字节码；编译失败时返回 <c>null</c>。</para>
    /// </returns>
    private static unsafe byte[]? CompileGlsl(byte[] source, ShaderKind shaderKind, string fileName)
    {
        // Resolve the shaderc bindings lazily; loading the native library may fail.
        // 延迟获取 shaderc 绑定；原生库可能加载失败。
        ShadercApi api;
        try
        {
            api = Api.Value;
        }
        catch (Exception e)
        {
            LogCat.Error($"Failed to compile shader: the shader compiler is unavailable. {e.Message}");
            return null;
        }

        // A compiler instance owns the native compilation state.
        // 编译器实例持有原生编译状态。
        var compiler = api.CompilerInitialize();
        if (compiler == null)
        {
            LogCat.Error("Failed to compile shader: could not initialize the shader compiler.");
            return null;
        }

        // The options object is mutated below and then passed to CompileIntoSpv.
        // 下面的选项对象会被修改，然后传给 CompileIntoSpv。
        var options = api.CompileOptionsInitialize();
        if (options == null)
        {
            LogCat.Error("Failed to compile shader: could not initialize the compile options.");
            api.CompilerRelease(compiler);
            return null;
        }

        try
        {
            // Configure GLSL input and Vulkan 1.0 compatible validation rules.
            // 配置 GLSL 输入和 Vulkan 1.0 兼容的校验规则。
            api.CompileOptionsSetSourceLanguage(options, SourceLanguage.Glsl);
            api.CompileOptionsSetTargetEnv(options, TargetEnv.Vulkan, (uint)EnvVersion.Vulkan10);
            api.CompileOptionsSetOptimizationLevel(options, OptimizationLevel.Performance);

            // Native strings must be NUL-terminated UTF-8 byte arrays.
            // 原生字符串必须是以 \0 结尾的 UTF-8 字节数组。
            var displayName = string.IsNullOrWhiteSpace(fileName) ? "<memory>" : fileName;
            var fileNameBytes = Encoding.UTF8.GetBytes(displayName + '\0');
            var entryPointBytes = Encoding.UTF8.GetBytes(DefaultEntryPoint + '\0');

            // Pin managed arrays for the duration of the native call.
            // 在原生调用期间固定托管数组，避免 GC 移动内存。
            CompilationResult* result;
            fixed (byte* sourcePtr = source)
            fixed (byte* fileNamePtr = fileNameBytes)
            fixed (byte* entryPointPtr = entryPointBytes)
            {
                result = api.CompileIntoSpv(compiler, sourcePtr, (nuint)source.Length,
                    shaderKind, fileNamePtr, entryPointPtr, options);
            }

            if (result == null)
            {
                LogCat.Error("Failed to compile shader: the compiler returned no result.");
                return null;
            }

            // The compilation result is a native object and must be released explicitly.
            // 编译结果是原生对象，必须显式释放。
            try
            {
                // A non-success status means the GLSL did not compile.
                // 状态不是 Success 表示 GLSL 编译失败。
                var status = api.ResultGetCompilationStatus(result);
                if (status != CompilationStatus.Success)
                {
                    // shaderc reports human-readable diagnostics through the result.
                    // shaderc 通过结果对象提供可读的诊断信息。
                    var message = Marshal.PtrToStringUTF8((nint)api.ResultGetErrorMessage(result));
                    LogCat.Error($"Failed to compile shader ({status})." +
                                 (string.IsNullOrWhiteSpace(message) ? string.Empty : $"\n{message}"));
                    return null;
                }

                // Copy the SPIR-V payload into managed memory before releasing the result.
                // 在释放结果前，把 SPIR-V 数据复制到托管内存。
                var length = api.ResultGetLength(result);
                if (length == 0 || length > int.MaxValue)
                {
                    LogCat.Error("Failed to compile shader: the compiler returned an empty or oversized result.");
                    return null;
                }

                var spirv = new byte[(int)length];
                Marshal.Copy((nint)api.ResultGetBytes(result), spirv, 0, spirv.Length);

                // Basic sanity check: a valid module starts with the SPIR-V magic number.
                // 基本校验：合法的模块以 SPIR-V 魔数开头。
                if (spirv.Length < sizeof(uint) ||
                    BinaryPrimitives.ReadUInt32LittleEndian(spirv) != SpirvMagic)
                {
                    LogCat.Error("Failed to compile shader: the result is not valid SPIR-V.");
                    return null;
                }

                return spirv;
            }
            finally
            {
                api.ResultRelease(result);
            }
        }
        finally
        {
            // Release native resources even when compilation fails.
            // 即使编译失败，也要释放原生资源。
            api.CompileOptionsRelease(options);
            api.CompilerRelease(compiler);
        }
    }

    /// <summary>
    /// <para>Counts the shader resources declared in a SPIR-V module.</para>
    /// <para>统计 SPIR-V 模块中声明的着色器资源数量。</para>
    /// </summary>
    /// <param name="spirv">
    /// <para>The SPIR-V bytecode to inspect.</para>
    /// <para>要检查的 SPIR-V 字节码。</para>
    /// </param>
    /// <returns>
    /// <para>The resource counts used by <c>SDL.GPUShaderCreateInfo</c>.</para>
    /// <para>供 <c>SDL.GPUShaderCreateInfo</c> 使用的资源数量。</para>
    /// </returns>
    private static ShaderResourceCounts ReadResourceCounts(byte[] spirv)
    {
        var types = new Dictionary<uint, SpirvTypeInfo>();
        var constants = new Dictionary<uint, ulong>();
        var bufferBlockTypes = new HashSet<uint>();
        var variables = new List<(uint TypeId, uint StorageClass)>();

        var totalWords = spirv.Length / sizeof(uint);
        var wordIndex = SpirvHeaderSize / sizeof(uint);

        while (wordIndex < totalWords)
        {
            var instruction = ReadWord(spirv, wordIndex);
            var wordCount = (int)(instruction >> 16);
            var opcode = (int)(instruction & 0xFFFF);

            if (wordCount <= 0 || wordIndex + wordCount > totalWords)
            {
                break;
            }

            var operand = wordIndex + 1;
            switch (opcode)
            {
                case OpTypeImage when wordCount >= 9:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.Image,
                        // Sampled: 1 = sampled image, 2 = storage image.
                        // Sampled：1 表示采样图像，2 表示存储图像。
                        Sampled = ReadWord(spirv, operand + 6)
                    };
                    break;
                case OpTypeSampler when wordCount >= 2:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.Sampler
                    };
                    break;
                case OpTypeSampledImage when wordCount >= 3:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.SampledImage,
                        ElementTypeId = ReadWord(spirv, operand + 1)
                    };
                    break;
                case OpTypeArray when wordCount >= 4:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.Array,
                        ElementTypeId = ReadWord(spirv, operand + 1),
                        LengthId = ReadWord(spirv, operand + 2)
                    };
                    break;
                case OpTypeRuntimeArray when wordCount >= 3:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.RuntimeArray,
                        ElementTypeId = ReadWord(spirv, operand + 1)
                    };
                    break;
                case OpTypeStruct when wordCount >= 2:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.Struct
                    };
                    break;
                case OpTypePointer when wordCount >= 4:
                    types[ReadWord(spirv, operand)] = new SpirvTypeInfo
                    {
                        Kind = SpirvTypeKind.Pointer,
                        ElementTypeId = ReadWord(spirv, operand + 2)
                    };
                    break;
                case OpConstant when wordCount >= 4:
                    constants[ReadWord(spirv, operand + 1)] = ReadWord(spirv, operand + 2);
                    break;
                case OpVariable when wordCount >= 4:
                    variables.Add((ReadWord(spirv, operand), ReadWord(spirv, operand + 2)));
                    break;
                case OpDecorate when wordCount >= 3:
                    if (ReadWord(spirv, operand + 1) == DecorationBufferBlock)
                    {
                        bufferBlockTypes.Add(ReadWord(spirv, operand));
                    }

                    break;
            }

            wordIndex += wordCount;
        }

        // BufferBlock marks a Uniform struct as a storage buffer in SPIR-V 1.0.
        // BufferBlock 表示 SPIR-V 1.0 中 Uniform 存储类下的结构体是存储缓冲。
        foreach (var typeId in bufferBlockTypes)
        {
            if (types.TryGetValue(typeId, out var type))
            {
                type.IsBufferBlock = true;
                types[typeId] = type;
            }
        }

        uint samplers = 0;
        uint uniformBuffers = 0;
        uint storageTextures = 0;
        uint storageBuffers = 0;

        foreach (var (typeId, storageClass) in variables)
        {
            var multiplicity = 1u;
            ResolveResourceType(typeId, types, constants, ref multiplicity, out var typeInfo);

            switch (storageClass)
            {
                case StorageClassUniformConstant:
                    if (typeInfo.Kind is SpirvTypeKind.SampledImage or SpirvTypeKind.Sampler)
                    {
                        samplers += multiplicity;
                    }
                    else if (typeInfo.Kind == SpirvTypeKind.Image)
                    {
                        if (typeInfo.Sampled == 2)
                        {
                            storageTextures += multiplicity;
                        }
                        else
                        {
                            samplers += multiplicity;
                        }
                    }

                    break;
                case StorageClassUniform:
                    if (typeInfo.Kind == SpirvTypeKind.Struct && typeInfo.IsBufferBlock)
                    {
                        storageBuffers += multiplicity;
                    }
                    else
                    {
                        uniformBuffers += multiplicity;
                    }

                    break;
                case StorageClassStorageBuffer:
                    storageBuffers += multiplicity;
                    break;
            }
        }

        return new ShaderResourceCounts(samplers, uniformBuffers, storageTextures, storageBuffers);
    }

    /// <summary>
    /// <para>
    /// Resolves a variable type through pointers and arrays to the resource type it represents.
    /// </para>
    /// <para>
    /// 沿着指针和数组解析变量类型，得到它实际表示的资源类型。
    /// </para>
    /// </summary>
    /// <param name="typeId">
    /// <para>The SPIR-V result id of the variable type.</para>
    /// <para>变量类型对应的 SPIR-V 结果 ID。</para>
    /// </param>
    /// <param name="types">
    /// <para>The collected SPIR-V type definitions.</para>
    /// <para>已收集的 SPIR-V 类型定义。</para>
    /// </param>
    /// <param name="constants">
    /// <para>The collected SPIR-V integer constants, used for array lengths.</para>
    /// <para>已收集的 SPIR-V 整数常量，用于数组长度。</para>
    /// </param>
    /// <param name="multiplicity">
    /// <para>The number of resource slots represented by the variable.</para>
    /// <para>该变量占用的资源槽位数量。</para>
    /// </param>
    /// <param name="typeInfo">
    /// <para>The resolved resource type.</para>
    /// <para>解析后的资源类型。</para>
    /// </param>
    private static void ResolveResourceType(uint typeId, Dictionary<uint, SpirvTypeInfo> types,
        Dictionary<uint, ulong> constants, ref uint multiplicity, out SpirvTypeInfo typeInfo)
    {
        typeInfo = default;
        var currentTypeId = typeId;

        for (var depth = 0; depth < 32; depth++)
        {
            if (!types.TryGetValue(currentTypeId, out typeInfo))
            {
                return;
            }

            switch (typeInfo.Kind)
            {
                case SpirvTypeKind.Pointer:
                    currentTypeId = typeInfo.ElementTypeId;
                    continue;
                case SpirvTypeKind.Array:
                    var length = constants.TryGetValue(typeInfo.LengthId, out var constant)
                        ? (uint)constant
                        : 1u;
                    multiplicity = MultiplyMultiplicity(multiplicity, length);
                    currentTypeId = typeInfo.ElementTypeId;
                    continue;
                case SpirvTypeKind.RuntimeArray:
                    currentTypeId = typeInfo.ElementTypeId;
                    continue;
                default:
                    return;
            }
        }

        // The type chain is unexpectedly deep; treat it as unknown.
        // 类型链异常过长，按未知类型处理。
        typeInfo = default;
    }

    private static uint MultiplyMultiplicity(uint left, uint right)
    {
        if (left == 0 || right == 0)
        {
            return 0;
        }

        return left > uint.MaxValue / right ? uint.MaxValue : left * right;
    }

    private static uint ReadWord(byte[] spirv, int wordIndex)
    {
        return BinaryPrimitives.ReadUInt32LittleEndian(
            spirv.AsSpan(wordIndex * sizeof(uint), sizeof(uint)));
    }

    private enum SpirvTypeKind
    {
        Unknown,
        Image,
        Sampler,
        SampledImage,
        Array,
        RuntimeArray,
        Struct,
        Pointer
    }

    private struct SpirvTypeInfo
    {
        public SpirvTypeKind Kind;
        public uint Sampled;
        public uint ElementTypeId;
        public uint LengthId;
        public bool IsBufferBlock;
    }

    private readonly record struct ShaderResourceCounts(
        uint NumSamplers,
        uint NumUniformBuffers,
        uint NumStorageTextures,
        uint NumStorageBuffers);
}