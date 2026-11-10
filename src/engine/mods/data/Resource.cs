using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Data;

/// <summary>
/// <para>Resource</para>
/// <para>资源类</para>
/// </summary>
public class Resource
{
    [TomlPropertyName("resource_id")] public string ResourceId = string.Empty;
    public string PackId { get; set; } = string.Empty;
}

/// <summary>
/// <para>DimensionResource</para>
/// <para>维度资源</para>
/// </summary>
public class DimensionResource : Resource
{
    /// <summary>
    /// <para>AllowAsStarting</para>
    /// <para>是否作为初始维度</para>
    /// </summary>
    [TomlPropertyName("allow_as_starting")]
    public bool AllowAsStarting = false;
}