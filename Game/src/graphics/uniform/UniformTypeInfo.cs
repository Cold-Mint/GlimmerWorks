using System.Collections.Frozen;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>Layout information of a uniform member type.</para>
/// <para>Uniform 成员类型的布局信息。</para>
/// </summary>
internal readonly record struct UniformTypeInfo(
    int Size,
    int Alignment,
    int ComponentCount,
    UniformValueKind Kind)
{
    /// <summary>
    /// <para>Layout information indexed by the normalized GLSL type name.</para>
    /// <para>按规范化后的 GLSL 类型名索引的布局信息。</para>
    /// </summary>
    /// <remarks>
    ///<para>The immutable dictionary type introduced in .NET 8 is specifically optimized for scenarios where data is read-only after creation and queried frequently.</para>
    ///<para>.NET 8 引入的不可变字典类型，专门为创建后只读、频繁查询的场景优化</para>
    /// </remarks>
    private static readonly FrozenDictionary<string, UniformTypeInfo> TypeInfos =
        new Dictionary<string, UniformTypeInfo>(StringComparer.Ordinal)
        {
            ["float"] = new(4, 4, 1, UniformValueKind.Float),
            ["int"] = new(4, 4, 1, UniformValueKind.Int),
            ["uint"] = new(4, 4, 1, UniformValueKind.UInt),
            ["bool"] = new(4, 4, 1, UniformValueKind.Bool),
            ["vec2"] = new(8, 8, 2, UniformValueKind.Float),
            ["vec3"] = new(12, 16, 3, UniformValueKind.Float),
            ["vec4"] = new(16, 16, 4, UniformValueKind.Float),
            ["ivec2"] = new(8, 8, 2, UniformValueKind.Int),
            ["ivec3"] = new(12, 16, 3, UniformValueKind.Int),
            ["ivec4"] = new(16, 16, 4, UniformValueKind.Int),
            ["uvec2"] = new(8, 8, 2, UniformValueKind.UInt),
            ["uvec3"] = new(12, 16, 3, UniformValueKind.UInt),
            ["uvec4"] = new(16, 16, 4, UniformValueKind.UInt),
            ["mat4"] = new(64, 16, 16, UniformValueKind.Float),
        }.ToFrozenDictionary(StringComparer.Ordinal);

    /// <summary>
    /// <para>Parse a GLSL type name into layout information.</para>
    /// <para>把 GLSL 类型名解析为布局信息。</para>
    /// </summary>
    public static UniformTypeInfo? TryParse(string? typeName)
    {
        if (string.IsNullOrEmpty(typeName))
        {
            return null;
        }

        if (TypeInfos.TryGetValue(typeName, out var info))
        {
            return info;
        }

        return null;
    }
}