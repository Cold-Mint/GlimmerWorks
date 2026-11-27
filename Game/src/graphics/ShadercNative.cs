using System.Runtime.InteropServices;
using Silk.NET.Shaderc;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>
/// Safe P/Invoke bindings for the shaderc C API used by <see cref="ShaderCompiler"/>.
/// Native handles are exposed as <see cref="IntPtr"/>, so callers never need an
/// unsafe context.
/// </para>
/// <para>
/// <see cref="ShaderCompiler"/> 使用的 shaderc C API 安全 P/Invoke 绑定。
/// 原生句柄以 <see cref="IntPtr"/> 暴露，因此调用方无需 unsafe 上下文。
/// </para>
/// </summary>
/// <remarks>
/// <para>
/// Silk.NET's generated shaderc bindings are pointer-based. Declaring the few
/// functions we need directly against the C API keeps the rest of the compiler
/// in safe code, while still using the native library shipped by
/// <c>Silk.NET.Shaderc.Native</c>.
/// </para>
/// <para>
/// Silk.NET 生成的 shaderc 绑定基于指针。这里直接针对 C API 声明所需的少量函数，
/// 让编译器其余部分保持安全代码，同时仍然使用 <c>Silk.NET.Shaderc.Native</c>
/// 提供的原生库。
/// </para>
/// </remarks>
internal static class ShadercNative
{
    private const string LibraryName = "shaderc_shared";

    [DllImport(LibraryName, EntryPoint = "shaderc_compiler_initialize",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern IntPtr CompilerInitialize();

    [DllImport(LibraryName, EntryPoint = "shaderc_compiler_release",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void CompilerRelease(IntPtr compiler);

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_options_initialize",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern IntPtr CompileOptionsInitialize();

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_options_release",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void CompileOptionsRelease(IntPtr options);

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_options_set_source_language",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void CompileOptionsSetSourceLanguage(IntPtr options, SourceLanguage language);

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_options_set_target_env",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void CompileOptionsSetTargetEnv(IntPtr options, TargetEnv target, uint version);

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_options_set_optimization_level",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void CompileOptionsSetOptimizationLevel(IntPtr options, OptimizationLevel level);

    [DllImport(LibraryName, EntryPoint = "shaderc_compile_into_spv",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern IntPtr CompileIntoSpv(IntPtr compiler, [In] byte[] sourceText,
        nuint sourceTextSize, ShaderKind shaderKind,
        [MarshalAs(UnmanagedType.LPUTF8Str)] string inputFileName,
        [MarshalAs(UnmanagedType.LPUTF8Str)] string entryPointName, IntPtr options);

    [DllImport(LibraryName, EntryPoint = "shaderc_result_get_compilation_status",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern CompilationStatus ResultGetCompilationStatus(IntPtr result);

    [DllImport(LibraryName, EntryPoint = "shaderc_result_get_error_message",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern IntPtr ResultGetErrorMessage(IntPtr result);

    [DllImport(LibraryName, EntryPoint = "shaderc_result_get_length",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern nuint ResultGetLength(IntPtr result);

    [DllImport(LibraryName, EntryPoint = "shaderc_result_get_bytes",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern IntPtr ResultGetBytes(IntPtr result);

    [DllImport(LibraryName, EntryPoint = "shaderc_result_release",
        CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
    internal static extern void ResultRelease(IntPtr result);
}