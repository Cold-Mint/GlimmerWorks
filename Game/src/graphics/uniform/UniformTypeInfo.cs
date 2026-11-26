namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>Layout information of a uniform member type.</para>
/// <para>Uniform 成员类型的布局信息。</para>
/// </summary>
internal readonly record struct UniformTypeInfo(
    int Size,
    int Alignment,
    int ComponentCount,
    UniformValueKind Kind,
    bool IsMatrix)
{
    /// <summary>
    /// <para>Parse a GLSL type name into layout information.</para>
    /// <para>把 GLSL 类型名解析为布局信息。</para>
    /// </summary>
    public static bool TryParse(string? typeName, out UniformTypeInfo info)
    {
        info = typeName?.Trim() switch
        {
            "float" => new UniformTypeInfo(4, 4, 1, UniformValueKind.Float, false),
            "int" => new UniformTypeInfo(4, 4, 1, UniformValueKind.Int, false),
            "uint" => new UniformTypeInfo(4, 4, 1, UniformValueKind.UInt, false),
            "bool" => new UniformTypeInfo(4, 4, 1, UniformValueKind.Bool, false),
            "vec2" => new UniformTypeInfo(8, 8, 2, UniformValueKind.Float, false),
            "vec3" => new UniformTypeInfo(12, 16, 3, UniformValueKind.Float, false),
            "vec4" => new UniformTypeInfo(16, 16, 4, UniformValueKind.Float, false),
            "ivec2" => new UniformTypeInfo(8, 8, 2, UniformValueKind.Int, false),
            "ivec3" => new UniformTypeInfo(12, 16, 3, UniformValueKind.Int, false),
            "ivec4" => new UniformTypeInfo(16, 16, 4, UniformValueKind.Int, false),
            "uvec2" => new UniformTypeInfo(8, 8, 2, UniformValueKind.UInt, false),
            "uvec3" => new UniformTypeInfo(12, 16, 3, UniformValueKind.UInt, false),
            "uvec4" => new UniformTypeInfo(16, 16, 4, UniformValueKind.UInt, false),
            "mat4" => new UniformTypeInfo(64, 16, 16, UniformValueKind.Float, true),
            _ => default
        };
        return info.ComponentCount > 0;
    }
}