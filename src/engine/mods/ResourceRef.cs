using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods;

public class ResourceRef
{
    public string RequesterPackId { get; init; } = string.Empty;
    [TomlPropertyName("target_pack_id")] public string TargetPackId { get; init; } = string.Empty;

    [TomlPropertyName("resource_id")] public string ResourceId { get; init; } = string.Empty;

    [TomlPropertyName("resource_type")] public string ResourceType { get; init; } = string.Empty;

    /// <summary>
    /// <para>IsValid</para>
    ///<para>是否合法</para>
    /// </summary>
    public bool IsValid => !string.IsNullOrEmpty(RequesterPackId) && !string.IsNullOrEmpty(ResourceId) &&
                           !string.IsNullOrEmpty(ResourceType);
}