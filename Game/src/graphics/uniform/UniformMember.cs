using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>A single member of a <see cref="UniformBlock"/>.</para>
/// <para><see cref="UniformBlock"/> 的单个成员。</para>
/// </summary>
public sealed class UniformMember
{
    /// <summary>
    /// <para>The member name inside the block.</para>
    /// <para>块内的成员名。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    /// <summary>
    /// <para>The GLSL type name, e.g. "vec4", "float", "mat4".</para>
    /// <para>GLSL 类型名，例如 "vec4"、"float"、"mat4"。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("type")]
    public string Type { get; set; } = string.Empty;

    /// <summary>
    /// <para>
    /// The value source: <c>@runtime.xxx</c> is injected by the engine, otherwise
    /// the member uses the static <see cref="Value"/>.
    /// </para>
    /// <para>
    /// 值来源：<c>@runtime.xxx</c> 由引擎注入，否则使用静态的 <see cref="Value"/>。
    /// </para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("source")]
    public string Source { get; set; } = string.Empty;

    /// <summary>
    /// <para>The static value components.</para>
    /// <para>静态值分量。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("value")]
    public List<double>? Value { get; set; }

    /// <summary>
    /// <para>The byte offset inside the packed block.</para>
    /// <para>打包块内的字节偏移。</para>
    /// </summary>
    public int Offset { get; internal set; }

    internal UniformTypeInfo TypeInfo { get; set; }
}