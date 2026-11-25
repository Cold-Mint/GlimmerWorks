namespace GlimmerWorks.Game.Attributes;

/// <summary>
/// <para>AssetsPasterAttribute</para>
/// <para>资源解析器</para>
/// </summary>
[AttributeUsage(AttributeTargets.Class, Inherited = false)]
public sealed class AssetsPasterAttribute(string resourceType) : Attribute
{
    public string ResourceType { get; } = resourceType.Length == 0
        ? throw new ArgumentException("resourceType cannot be empty")
        : resourceType;
}