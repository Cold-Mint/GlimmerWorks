using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Game.Graphics.Uniform;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>The blend modes a pipeline can use.</para>
/// <para>管线可使用的混合模式。</para>
/// </summary>
public enum BlendMode
{
    /// <summary>
    /// <para>No blending: the fragment color overwrites the target.</para>
    /// <para>不混合：片元颜色覆盖目标。</para>
    /// </summary>
    None = 0,

    /// <summary>
    /// <para>Straight alpha blending: src * srcAlpha + dst * (1 - srcAlpha).</para>
    /// <para>常规 Alpha 混合：src * srcAlpha + dst * (1 - srcAlpha)。</para>
    /// </summary>
    Alpha = 1,

    /// <summary>
    /// <para>Additive blending: src * srcAlpha + dst.</para>
    /// <para>加性混合：src * srcAlpha + dst。</para>
    /// </summary>
    Additive = 2
}

/// <summary>
/// <para>A material pipeline: blend mode, compiled shaders and uniform blocks.</para>
/// <para>材质管线：混合模式、编译后的着色器与 uniform 块。</para>
/// </summary>
public sealed class Pipeline
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

/// <summary>
/// <para>The TOML representation of a pipeline asset.</para>
/// <para>管线资产的 TOML 表示。</para>
/// </summary>
public sealed class PipelineDefinition
{
    [UsedImplicitly]
    [TomlPropertyName("blend_mode")]
    public int BlendMode { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("vertex_shader")]
    public ResourceRef? VertexShader { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("fragment_shader")]
    public ResourceRef? FragmentShader { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("vertex_uniform_block")]
    public List<ResourceRef> VertexUniformBlocks { get; set; } = [];

    [UsedImplicitly]
    [TomlPropertyName("fragment_uniform_block")]
    public List<ResourceRef> FragmentUniformBlocks { get; set; } = [];
}
