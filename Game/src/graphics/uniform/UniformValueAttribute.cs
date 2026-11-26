namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>
/// Marks a public static field or property as a runtime uniform value. The
/// <see cref="RuntimeUniformManager"/> discovers marked members through reflection.
/// </para>
/// <para>
/// 标记一个公开的静态字段或属性为运行时 uniform 值。
/// <see cref="RuntimeUniformManager"/> 会通过反射发现这些成员。
/// </para>
/// </summary>
/// <remarks>
/// <para>
/// When no key is given, the key is the snake_case form of the member name with
/// any leading underscores removed, e.g. <c>ViewSize</c> becomes
/// <c>view_size</c>. That key can then be used as a uniform block source:
/// <c>source = "@runtime.view_size"</c>.
/// </para>
/// <para>
/// 未指定键时，键为成员名去掉前导下划线后的蛇形命名，例如 <c>ViewSize</c>
/// 会变成 <c>view_size</c>。这个键可以在 uniform 块中作为
/// <c>source = "@runtime.view_size"</c> 使用。
/// </para>
/// </remarks>
[AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
public sealed class UniformValueAttribute : Attribute
{
    /// <summary>
    /// <para>The explicit runtime key. When <c>null</c>, the member name is used.</para>
    /// <para>显式的运行时键。为 <c>null</c> 时使用成员名。</para>
    /// </summary>
    public string? Key { get; }

    /// <summary>
    /// <para>Use the snake_case member name as the key.</para>
    /// <para>使用成员的蛇形命名作为键。</para>
    /// </summary>
    public UniformValueAttribute()
    {
    }

    /// <summary>
    /// <para>Use an explicit key.</para>
    /// <para>使用显式指定的键。</para>
    /// </summary>
    /// <param name="key"></param>
    public UniformValueAttribute(string key)
    {
        Key = key;
    }
}
