using GlimmerWorks.Game.Graphics.Uniform;
namespace GlimmerWorks.Game.Graphics.Pipeline;

/// <summary>
/// <para>A material pipeline: blend mode, compiled shaders and uniform blocks.</para>
/// <para>材质管线：混合模式、编译后的着色器与 uniform 块。</para>
/// </summary>
public sealed class GPUPipeline
{
    /// <summary>
    /// <para>The blend mode (see <see cref="BlendMode"/>).</para>
    /// <para>混合模式（见 <see cref="BlendMode"/>）。</para>
    /// </summary>
    public BlendMode BlendMode { get; set; } = BlendMode.None;

    public CompiledShader? VertexShader { get; set; }

    public CompiledShader? FragmentShader { get; set; }

    /// <summary>
    /// <para>The vertex uniform blocks in shader declaration order.</para>
    /// <para>按着色器声明顺序排列的顶点 uniform 块。</para>
    /// </summary>
    public List<UniformBlock> VertexUniformBlocks { get; } = [];

    /// <summary>
    /// <para>The fragment uniform blocks in shader declaration order.</para>
    /// <para>按着色器声明顺序排列的片元 uniform 块。</para>
    /// </summary>
    public List<UniformBlock> FragmentUniformBlocks { get; } = [];
}
