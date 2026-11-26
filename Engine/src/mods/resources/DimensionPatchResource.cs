using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>DimensionResourcePatch</para>
/// <para>维度资源补丁</para>
/// </summary>
[UsedImplicitly]
[DataPackResource("dimension_patch", patch: true)]
public class DimensionPatchResource : Resource, IResourcePatch
{
    [UsedImplicitly] [TomlPropertyName("dimension")]
    public ResourceRef Dimension = new();

    /// <summary>
    /// <para>Add certain biomes to the dimension</para>
    /// <para>把某些生物群系添加到维度内</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("append_biomes")]
    public List<ResourceRef> AppendBiomes { get; set; } = [];

    public void ApplyPatch(DataRepository dataRepository)
    {
        var dimensionResource = dataRepository.Get<DimensionResource>(Dimension);
        if (dimensionResource == null)
        {
            return;
        }

        dimensionResource.Biomes.AddRange(AppendBiomes);
    }

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        Dimension.SetPackId(selfPackId);
        foreach (var appendBiome in AppendBiomes)
        {
            appendBiome.SetPackId(selfPackId);
        }
    }
}