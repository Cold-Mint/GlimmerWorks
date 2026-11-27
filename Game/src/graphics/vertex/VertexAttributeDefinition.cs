using JetBrains.Annotations;
using SDL3;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics.Vertex;

/// <summary>
/// <para>A single vertex attribute of a <see cref="VertexLayout"/>.</para>
/// <para><see cref="VertexLayout"/> 的单个顶点属性。</para>
/// </summary>
public sealed class VertexAttributeDefinition
{
    /// <summary>
    /// <para>The shader input location (<c>layout(location = N)</c>).</para>
    /// <para>着色器输入位置（<c>layout(location = N)</c>）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("location")]
    public uint Location { get; set; }

    /// <summary>
    /// <para>The semantic name: <c>position</c>, <c>uv</c> or <c>color</c>.</para>
    /// <para>语义名：<c>position</c>、<c>uv</c> 或 <c>color</c>。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("semantic")]
    public string Semantic { get; set; } = string.Empty;

    /// <summary>
    /// <para>The element format, e.g. <c>float2</c> or <c>float4</c>.</para>
    /// <para>元素格式，例如 <c>float2</c> 或 <c>float4</c>。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("format")]
    public string Format { get; set; } = string.Empty;

    /// <summary>
    /// <para>The byte offset inside one vertex.</para>
    /// <para>单个顶点内的字节偏移。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("offset")]
    public int Offset { get; set; }

    /// <summary>
    /// <para>The resolved SDL element format.</para>
    /// <para>解析后的 SDL 元素格式。</para>
    /// </summary>
    [TomlIgnore]
    public VertexFormat VertexFormat { get; set; }
}