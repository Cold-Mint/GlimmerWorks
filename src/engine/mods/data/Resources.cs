using GlimmerWorks.Engine.Attributes;
using GlimmerWorks.Engine.Vfs;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Data;

/// <summary>
/// <para>Resource</para>
/// <para>资源类</para>
/// </summary>
public class Resources
{
    [UsedImplicitly]
    [TomlPropertyName("resource_id")]
    public string ResourceId { get; init; } = string.Empty;

    public string PackId { get; set; } = string.Empty;
    public VirtualPath? VirtualPath { get; set; }

    [UsedImplicitly]
    [TomlPropertyName("display_name")]
    public ResourceRef DisplayName { get; set; } = new();

    public virtual string GetDisplayName(AppContext appContext)
    {
        var stringResources = appContext.DataRepository.Get<StringResources>(DisplayName);
        if (stringResources == null)
        {
            return $"{PackId}:{ResourceId}";
        }

        return stringResources.Value;
    }

    public bool Filter(AppContext appContext, string keyString)
    {
        if (ResourceId.Contains(keyString))
        {
            return true;
        }

        if (PackId.Contains(keyString))
        {
            return true;
        }

        if (GetDisplayName(appContext).Contains(keyString))
        {
            return true;
        }

        return false;
    }
}

/// <summary>
/// <para>TemplateResources</para>
/// <para>模板资源</para>
/// </summary>
[DataPackResource("template", autoDeserialization: false)]
public class TemplateResources : Resources
{
    public string Content { get; init; } = string.Empty;
}

/// <summary>
/// <para>StringFileResources</para>
/// <para>字符串文件资源</para>
/// </summary>
public class StringSetResources : Resources
{
    [UsedImplicitly]
    [TomlPropertyName("string")]
    public List<StringResources> StringList { get; set; } = new();

    [UsedImplicitly]
    [TomlPropertyName("tag_string")]
    public List<TagStringResources> StringTagList { get; set; } = new();
}

/// <summary>
/// <para>StringResources</para>
/// <para>字符串资源</para>
/// </summary>
[DataPackResource("strings", autoDeserialization: false)]
public class StringResources : Resources
{
    [UsedImplicitly]
    [TomlPropertyName("value")]
    public string Value { get; set; } = string.Empty;

    public override string GetDisplayName(AppContext appContext)
    {
        return Value;
    }
}

/// <summary>
/// <para>TagStringResources</para>
/// <para>标签字符串资源</para>
/// </summary>
public class TagStringResources : Resources
{
    [UsedImplicitly]
    [TomlPropertyName("value")] 
    public string Value { get; set; } = string.Empty;

    public override string GetDisplayName(AppContext appContext)
    {
        return Value;
    }
}

/// <summary>
/// <para>DimensionResource</para>
/// <para>维度资源</para>
/// </summary>
[UsedImplicitly]
[DataPackResource("dimension")]
public class DimensionResources : Resources
{
    /// <summary>
    /// <para>AllowAsStarting</para>
    /// <para>是否作为初始维度</para>
    /// </summary>
    [TomlPropertyName("allow_as_starting")]
    public bool AllowAsStarting { get; set; } = false;
}

[DataPackResource("tile")]
public class TileResources : Resources
{
}