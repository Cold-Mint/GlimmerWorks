using GlimmerWorks.Engine.Mods;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>A material asset: a texture plus the pipeline used to draw it.</para>
/// <para>材质资产：一张纹理与用于绘制它的管线。</para>
/// </summary>
public sealed class Material
{
    /// <summary>
    /// <para>The resolved texture.</para>
    /// <para>解析后的纹理。</para>
    /// </summary>
    public TextureData? Texture { get; set; }

    /// <summary>
    /// <para>The resolved pipeline.</para>
    /// <para>解析后的管线。</para>
    /// </summary>
    public Pipeline? Pipeline { get; set; }
}

/// <summary>
/// <para>The TOML representation of a material asset.</para>
/// <para>材质资产的 TOML 表示。</para>
/// </summary>
public sealed class MaterialDefinition
{
    /// <summary>
    /// <para>The texture reference.</para>
    /// <para>纹理引用。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("texture")]
    public ResourceRef? Texture { get; set; }

    /// <summary>
    /// <para>The pipeline reference.</para>
    /// <para>管线引用。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("pipeline")]
    public ResourceRef? Pipeline { get; set; }
}
