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

        return new CompiledShader
        {
            Spirv = spirv,
            VertexShader = vertexShader
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
}