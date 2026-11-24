using System.Buffers.Binary;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using GlimmerWorks.Engine.Log;
using GlimmerWorks.Engine.Vfs;
using Silk.NET.Shaderc;
using ShadercApi = Silk.NET.Shaderc.Shaderc;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>Compiles GLSL shaders into SPIR-V at runtime and caches the result next to the shader source.</para>
/// <para>在运行时把 GLSL 着色器编译为 SPIR-V，并把结果缓存到着色器源文件的同级目录。</para>
/// </summary>
public class ShaderCompiler(VirtualFileSystem virtualFileSystem)
{
    /// <summary>
    /// <para>The default shader entry point.</para>
    /// <para>默认着色器入口函数名</para>
    /// </summary>
    public const string DefaultEntryPoint = "main";

    /// <summary>
    /// <para>The suffix appended to a shader source path to build the cache file path.</para>
    /// <para>缓存文件后缀，追加在着色器源文件路径之后（如 default.frag.cache）。</para>
    /// </summary>
    public const string CacheExtension = ".cache";

    /// <summary>
    /// <para>The default shader directory.</para>
    /// <para>默认着色器目录</para>
    /// </summary>
    public static readonly VirtualPath DefaultShaderDirectory = new("assets", "shaders");

    private const uint CacheMagic = 0x43535747; // "GWSC"
    private const uint CacheFormatVersion = 1;
    private const int SourceHashSize = 32;
    private const int CacheHeaderSize = 48;
    private const uint SpirvMagic = 0x07230203;

    private static readonly Lazy<ShadercApi> Api = new(ShadercApi.GetApi);

    /// <summary>
    /// <para>Compile a shader from the default shader directory.</para>
    /// <para>从默认着色器目录编译一个着色器。</para>
    /// </summary>
    /// <param name="shaderName">Shader file name without extension. 不含扩展名的着色器文件名。</param>
    /// <param name="stage">Shader stage. 着色器阶段。</param>
    /// <param name="entryPoint">Entry point function name. 入口函数名。</param>
    /// <returns>The compiled shader, or null on failure. 编译结果，失败时返回 null。</returns>
    public CompiledShader? Compile(string shaderName, ShaderStage stage, string entryPoint = DefaultEntryPoint)
    {
        var shaderPath = DefaultShaderDirectory.Combine($"{shaderName}{GetExtension(stage)}");
        return Compile(shaderPath, stage, entryPoint);
    }

    /// <summary>
    /// <para>Compile a shader, reusing a valid cache file when possible.</para>
    /// <para>编译一个着色器，缓存有效时直接读取缓存文件。</para>
    /// </summary>
    /// <param name="shaderPath">Shader source path. 着色器源文件路径。</param>
    /// <param name="stage">Shader stage. 着色器阶段。</param>
    /// <param name="entryPoint">Entry point function name. 入口函数名。</param>
    /// <returns>The compiled shader, or null on failure. 编译结果，失败时返回 null。</returns>
    public CompiledShader? Compile(VirtualPath shaderPath, ShaderStage stage, string entryPoint = DefaultEntryPoint)
    {
        string? source;
        try
        {
            source = virtualFileSystem.ReadAllText(shaderPath);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            LogCat.Error($"Failed to read shader source '{shaderPath.Path}': {e.Message}");
            return null;
        }

        if (string.IsNullOrWhiteSpace(source))
        {
            LogCat.Error($"Shader source is empty or missing: {shaderPath.Path}");
            return null;
        }

        var sourceBytes = Encoding.UTF8.GetBytes(source);
        var sourceHash = SHA256.HashData(sourceBytes);
        var cachePath = new VirtualPath($"{shaderPath.Path}{CacheExtension}");

        if (TryReadCache(cachePath, sourceHash, stage, entryPoint, out var cachedShader))
        {
            LogCat.Debug($"Loaded shader from cache: {cachePath.Path}");
            return cachedShader;
        }

        var spirv = CompileGlsl(sourceBytes, shaderPath.Path, stage, entryPoint);
        if (spirv == null)
        {
            return null;
        }

        WriteCache(cachePath, sourceHash, stage, spirv);
        LogCat.Info($"Compiled shader: {shaderPath.Path} -> {cachePath.Path} ({spirv.Length} bytes)");
        return new CompiledShader(stage, entryPoint, spirv, false);
    }

    /// <summary>
    /// <para>Get the source file extension of a shader stage.</para>
    /// <para>获取着色器阶段对应的源文件扩展名。</para>
    /// </summary>
    public static string GetExtension(ShaderStage stage) => stage switch
    {
        ShaderStage.Vertex => ".vert",
        ShaderStage.Fragment => ".frag",
        ShaderStage.Compute => ".comp",
        _ => throw new ArgumentOutOfRangeException(nameof(stage), stage, "Unknown shader stage.")
    };

    private static unsafe byte[]? CompileGlsl(byte[] sourceBytes, string fileName, ShaderStage stage,
        string entryPoint)
    {
        ShadercApi api;
        try
        {
            api = Api.Value;
        }
        catch (Exception e)
        {
            LogCat.Error($"Shader compiler is unavailable: {e.Message}");
            return null;
        }

        var compiler = api.CompilerInitialize();
        if (compiler == null)
        {
            LogCat.Error("Failed to initialize the shader compiler.");
            return null;
        }

        var options = api.CompileOptionsInitialize();
        if (options == null)
        {
            LogCat.Error("Failed to initialize the shader compile options.");
            api.CompilerRelease(compiler);
            return null;
        }

        try
        {
            api.CompileOptionsSetSourceLanguage(options, SourceLanguage.Glsl);
            api.CompileOptionsSetTargetEnv(options, TargetEnv.Vulkan, (uint)EnvVersion.Vulkan10);
            api.CompileOptionsSetOptimizationLevel(options, OptimizationLevel.Performance);

            var fileNameBytes = Encoding.UTF8.GetBytes(fileName + '\0');
            var entryPointBytes = Encoding.UTF8.GetBytes(entryPoint + '\0');
            CompilationResult* result;
            fixed (byte* sourcePtr = sourceBytes)
            fixed (byte* fileNamePtr = fileNameBytes)
            fixed (byte* entryPointPtr = entryPointBytes)
            {
                result = api.CompileIntoSpv(compiler, sourcePtr, (nuint)sourceBytes.Length, ToShaderKind(stage),
                    fileNamePtr, entryPointPtr, options);
            }

            if (result == null)
            {
                LogCat.Error($"Failed to compile shader '{fileName}'.");
                return null;
            }

            try
            {
                var status = api.ResultGetCompilationStatus(result);
                if (status != CompilationStatus.Success)
                {
                    var message = Marshal.PtrToStringUTF8((nint)api.ResultGetErrorMessage(result));
                    LogCat.Error($"Failed to compile shader '{fileName}' ({status})." +
                                 (string.IsNullOrWhiteSpace(message) ? string.Empty : $"\n{message}"));
                    return null;
                }

                var length = (int)api.ResultGetLength(result);
                if (length <= 0)
                {
                    LogCat.Error($"Shader compiler returned an empty result for '{fileName}'.");
                    return null;
                }

                var spirv = new byte[length];
                Marshal.Copy((nint)api.ResultGetBytes(result), spirv, 0, length);
                return spirv;
            }
            finally
            {
                api.ResultRelease(result);
            }
        }
        finally
        {
            api.CompileOptionsRelease(options);
            api.CompilerRelease(compiler);
        }
    }

    private static ShaderKind ToShaderKind(ShaderStage stage) => stage switch
    {
        ShaderStage.Vertex => ShaderKind.GlslVertexShader,
        ShaderStage.Fragment => ShaderKind.GlslFragmentShader,
        ShaderStage.Compute => ShaderKind.GlslComputeShader,
        _ => throw new ArgumentOutOfRangeException(nameof(stage), stage, "Unknown shader stage.")
    };

    private bool TryReadCache(VirtualPath cachePath, ReadOnlySpan<byte> sourceHash, ShaderStage stage,
        string entryPoint, out CompiledShader? shader)
    {
        shader = null;

        if (!virtualFileSystem.FileExists(cachePath))
        {
            return false;
        }

        byte[]? cacheBytes;
        try
        {
            cacheBytes = virtualFileSystem.ReadAllBytes(cachePath);
        }
        catch (FileNotFoundException)
        {
            return false;
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            LogCat.Warn($"Failed to read shader cache '{cachePath.Path}': {e.Message}");
            return false;
        }

        if (cacheBytes is not { Length: >= CacheHeaderSize + sizeof(uint) })
        {
            return false;
        }

        var span = cacheBytes.AsSpan();
        if (BinaryPrimitives.ReadUInt32LittleEndian(span) != CacheMagic ||
            BinaryPrimitives.ReadUInt32LittleEndian(span[4..]) != CacheFormatVersion)
        {
            LogCat.Debug($"Ignoring shader cache with an unknown format: {cachePath.Path}");
            return false;
        }

        if (BinaryPrimitives.ReadUInt32LittleEndian(span[8..]) != (uint)stage)
        {
            LogCat.Debug($"Ignoring shader cache for a different stage: {cachePath.Path}");
            return false;
        }

        if (!span.Slice(12, SourceHashSize).SequenceEqual(sourceHash))
        {
            LogCat.Debug($"Shader cache is stale and will be rebuilt: {cachePath.Path}");
            return false;
        }

        var payloadLength = BinaryPrimitives.ReadUInt32LittleEndian(span[44..]);
        if (payloadLength != (uint)(cacheBytes.Length - CacheHeaderSize))
        {
            LogCat.Debug($"Ignoring truncated shader cache: {cachePath.Path}");
            return false;
        }

        var spirv = span[CacheHeaderSize..];
        if (payloadLength < sizeof(uint) || BinaryPrimitives.ReadUInt32LittleEndian(spirv) != SpirvMagic)
        {
            LogCat.Debug($"Ignoring shader cache with invalid SPIR-V data: {cachePath.Path}");
            return false;
        }

        shader = new CompiledShader(stage, entryPoint, spirv.ToArray(), true);
        return true;
    }

    private void WriteCache(VirtualPath cachePath, ReadOnlySpan<byte> sourceHash, ShaderStage stage,
        ReadOnlySpan<byte> spirv)
    {
        var cacheBytes = new byte[CacheHeaderSize + spirv.Length];
        var span = cacheBytes.AsSpan();
        BinaryPrimitives.WriteUInt32LittleEndian(span, CacheMagic);
        BinaryPrimitives.WriteUInt32LittleEndian(span[4..], CacheFormatVersion);
        BinaryPrimitives.WriteUInt32LittleEndian(span[8..], (uint)stage);
        sourceHash.CopyTo(span[12..]);
        BinaryPrimitives.WriteUInt32LittleEndian(span[44..], (uint)spirv.Length);
        spirv.CopyTo(span[CacheHeaderSize..]);

        try
        {
            virtualFileSystem.WriteAllBytes(cachePath, cacheBytes);
        }
        catch (Exception e) when (e is IOException or UnauthorizedAccessException)
        {
            LogCat.Warn($"Failed to write shader cache '{cachePath.Path}': {e.Message}");
        }
    }
}
