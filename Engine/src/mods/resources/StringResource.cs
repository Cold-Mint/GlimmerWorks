using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>StringResources</para>
/// <para>字符串资源</para>
/// </summary>
[DataPackResource("strings", autoDeserialization: false)]
public class StringResource : Resource
{
    [UsedImplicitly]
    [TomlPropertyName("value")]
    public string Value { get; set; } = string.Empty;

    public override string GetDisplayName(AppContext appContext)
    {
        return Value;
    }
}