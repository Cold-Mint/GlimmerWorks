using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods;

public class ResourceRef
{
    public string RequesterPackId { get; set; } = string.Empty;

    [TomlPropertyName("target_pack_id")] public string TargetPackId { get; set; } = string.Empty;

    [TomlPropertyName("resource_id")] public string ResourceId { get; init; } = string.Empty;

    [TomlPropertyName("resource_type")] public string ResourceType { get; init; } = string.Empty;

    /// <summary>
    /// <para>IsValid</para>
    ///<para>是否合法</para>
    /// </summary>
    public bool IsValid => !string.IsNullOrEmpty(RequesterPackId) && !string.IsNullOrEmpty(ResourceId) &&
                           !string.IsNullOrEmpty(ResourceType);

    /// <summary>
    /// <para>FromString</para>
    /// <para>从字符串表示的形式中构建资源引用</para>
    /// </summary>
    /// <param name="shot">
    ///<para>Resource Type: Target Pack ID: Resource ID</para>
    ///<para>格式：ResourceType:TargetPackId:ResourceId </para>
    /// </param>
    /// <param name="requesterPackId"></param>
    /// <returns></returns>
    public static ResourceRef? FromString(string shot, string requesterPackId)
    {
        var array = shot.Split(':');
        if (array.Length < 3)
        {
            return null;
        }

        ResourceRef resourceRef = new()
        {
            ResourceType = array[0],
            TargetPackId = array[1],
            ResourceId = array[2]
        };
        resourceRef.SetPackId(requesterPackId);
        return resourceRef;
    }

    /// <summary>
    /// <para>SetPackId</para>
    /// <para>设置包Id</para>
    /// </summary>
    /// <param name="packId"></param>
    public void SetPackId(string packId)
    {
        RequesterPackId = packId;
        if (string.IsNullOrWhiteSpace(TargetPackId) || TargetPackId == "@self")
        {
            TargetPackId = packId;
        }
    }
}