using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>TagStringResources</para>
/// <para>标签字符串资源</para>
/// </summary>
public class TagStringResource : Resource
{
    [UsedImplicitly]
    [TomlPropertyName("value")]
    public string Value { get; set; } = string.Empty;

    public override string GetDisplayName(AppContext appContext)
    {
        return Value;
    }
}