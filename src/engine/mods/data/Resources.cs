using GlimmerWorks.engine;
using GlimmerWorks.engine.Vfs;
using Microsoft.Extensions.Localization;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Data;

/// <summary>
/// <para>Resource</para>
/// <para>资源类</para>
/// </summary>
public class Resources
{
    [TomlPropertyName("resource_id")] public string ResourceId { get; set; } = string.Empty;
    public string PackId { get; set; } = string.Empty;
    public VirtualPath? VirtualPath { get; set; }
}

/// <summary>
/// <para>TemplateResources</para>
/// <para>模板资源</para>
/// </summary>
[DataPackResource("template", autoDeserialization: false)]
public class TemplateResources : Resources
{
    public string Content { get; set; } = string.Empty;
}

/// <summary>
/// <para>StringFileResources</para>
/// <para>字符串文件资源</para>
/// </summary>
public class StringSetResources : Resources
{
    [TomlPropertyName("string")] public List<StringResources> StringList { get; set; } = new();
    [TomlPropertyName("tag_string")] public List<TagStringResources> StringTagList { get; set; } = new();
}

/// <summary>
/// <para>StringResources</para>
/// <para>字符串资源</para>
/// </summary>
[DataPackResource("string", autoDeserialization: false)]
public class StringResources : Resources
{
    [TomlPropertyName("value")] public string Value { get; set; } = string.Empty;
}

/// <summary>
/// <para>TagStringResources</para>
/// <para>标签字符串资源</para>
/// </summary>
public class TagStringResources : Resources
{
    [TomlPropertyName("value")] public string Value { get; set; } = string.Empty;
}

/// <summary>
/// <para>DimensionResource</para>
/// <para>维度资源</para>
/// </summary>
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