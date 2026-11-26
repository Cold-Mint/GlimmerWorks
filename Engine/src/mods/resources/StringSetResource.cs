using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>StringFileResources</para>
/// <para>字符串文件资源</para>
/// </summary>
public class StringSetResource : Resource
{
    [UsedImplicitly]
    [TomlPropertyName("string")]
    public List<StringResource> StringList { get; set; } = new();

    [UsedImplicitly]
    [TomlPropertyName("tag_string")]
    public List<TagStringResource> StringTagList { get; set; } = new();
}