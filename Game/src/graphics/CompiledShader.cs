namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>CompiledShader</para>
/// <para>编译完成的着色器，内容为 SPIR-V 字节码。</para>
/// </summary>
public sealed class CompiledShader(ShaderStage stage, string entryPoint, byte[] spirv, bool fromCache)
{
    /// <summary>
    /// <para>The stage of this shader.</para>
    /// <para>着色器阶段</para>
    /// </summary>
    public ShaderStage Stage { get; } = stage;

    /// <summary>
    /// <para>The entry point function name.</para>
    /// <para>入口函数名</para>
    /// </summary>
    public string EntryPoint { get; } = entryPoint;

    /// <summary>
    /// <para>SPIR-V bytecode.</para>
    /// <para>SPIR-V 字节码</para>
    /// </summary>
    public byte[] Spirv { get; } = spirv;

    /// <summary>
    /// <para>Whether the bytecode was loaded from the cache file instead of being compiled.</para>
    /// <para>字节码是否来自缓存文件，而不是本次运行时编译的结果。</para>
    /// </summary>
    public bool FromCache { get; } = fromCache;

    /// <summary>
    /// <para>The size of the SPIR-V bytecode in bytes.</para>
    /// <para>SPIR-V 字节码大小（字节）</para>
    /// </summary>
    public int Size => Spirv.Length;
}
