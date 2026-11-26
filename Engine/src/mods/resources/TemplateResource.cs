using GlimmerWorks.Engine.Attributes;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>TemplateResource</para>
/// <para>模板资源</para>
/// </summary>
[DataPackResource("template", autoDeserialization: false)]
public class TemplateResource : Resource
{
    public string Content { get; init; } = string.Empty;
}