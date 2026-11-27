namespace GlimmerWorks.Game.Graphics.Pipeline;

using GlimmerWorks.Engine.Mods;
using JetBrains.Annotations;
using Tomlyn.Serialization;

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
    [TomlPropertyName("vertex_layout")]
    public ResourceRef? VertexLayout { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("vertex_uniform_block")]
    public List<ResourceRef> VertexUniformBlocks { get; set; } = [];

    [UsedImplicitly]
    [TomlPropertyName("fragment_uniform_block")]
    public List<ResourceRef> FragmentUniformBlocks { get; set; } = [];
}