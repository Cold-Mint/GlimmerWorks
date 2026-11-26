using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.Vfs;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods;

/// <summary>
/// <para>Resource</para>
/// <para>资源类</para>
/// </summary>
public class Resource
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
        var stringResources = appContext.DataRepository.Get<StringResource>(DisplayName);
        if (stringResources == null)
        {
            return $"{PackId}:{ResourceId}";
        }

        return stringResources.Value;
    }

    public virtual void BeforeRegister(string selfPackId)
    {
        DisplayName.SetPackId(selfPackId);
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