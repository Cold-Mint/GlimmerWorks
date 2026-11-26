using MemoryPack;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>CompiledShader</para>
/// <para>编译完成的着色器，内容为 SPIR-V 字节码。</para>
/// </summary>
[MemoryPackable]
public partial class CompiledShader
{
    /// <summary>
    /// <para>SPIR-V bytecode.</para>
    /// <para>SPIR-V 字节码</para>
    /// </summary>
    public byte[]? Spirv { get; set; }

    public bool VertexShader { get; set; }

    /// <summary>
    /// <para>The number of samplers defined in the shader.</para>
    /// <para>着色器中定义的采样纹理数量。</para>
    /// </summary>
    public uint NumSamplers { get; set; }

    /// <summary>
    /// <para>The number of uniform buffers defined in the shader.</para>
    /// <para>着色器中定义的 Uniform Buffer 数量。</para>
    /// </summary>
    public uint NumUniformBuffers { get; set; }

    /// <summary>
    /// <para>The number of storage textures defined in the shader.</para>
    /// <para>着色器中定义的存储纹理数量。</para>
    /// </summary>
    public uint NumStorageTextures { get; set; }

    /// <summary>
    /// <para>The number of storage buffers defined in the shader.</para>
    /// <para>着色器中定义的存储缓冲数量。</para>
    /// </summary>
    public uint NumStorageBuffers { get; set; }

    /// <summary>
    /// <para>The size of the SPIR-V bytecode in bytes.</para>
    /// <para>SPIR-V 字节码大小（字节）</para>
    /// </summary>
    public int Size => Spirv?.Length ?? 0;
}