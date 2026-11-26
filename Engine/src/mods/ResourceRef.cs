using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods;

public static class ResourceType
{
    public static readonly string FragShader = "shader_frag";
    public static readonly string VertShader = "shader_vert";
    public static readonly string Material = "material";
    public static readonly string Pipeline = "pipeline";
    public static readonly string Texture = "texture";
    public static readonly string UniformBlock = "uniform_block";
}

public class ResourceRef
{
    public static readonly string CorePackId = "@core";

    public string RequesterPackId { get; private set; } = string.Empty;

    [UsedImplicitly]
    [TomlPropertyName("target_pack_id")]
    public string TargetPackId { get; set; } = string.Empty;

    [UsedImplicitly]
    [TomlPropertyName("resource_id")]
    public string ResourceId { get; set; } = string.Empty;

    [UsedImplicitly]
    [TomlPropertyName("resource_type")]
    public string ResourceType { get; set; } = string.Empty;

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
    /// <para>Create</para>
    /// <para>创建资源引用</para>
    /// </summary>
    /// <param name="resourceType"></param>
    /// <param name="targetPackId">
    ///<para>The pack that owns the resource. "@self" means the requester pack.</para>
    ///<para>资源所在的资产包 ID。"@self" 表示请求者所在的资产包。</para>
    /// </param>
    /// <param name="resourceId"></param>
    /// <returns></returns>
    public static ResourceRef Create(string resourceType,
        string targetPackId,
        string resourceId)
    {
        ResourceRef resourceRef = new()
        {
            ResourceType = resourceType,
            TargetPackId = targetPackId,
            ResourceId = resourceId
        };
        resourceRef.SetPackId(CorePackId);
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
        if (string.IsNullOrEmpty(TargetPackId) || TargetPackId == "@self")
        {
            TargetPackId = packId;
        }
    }
}