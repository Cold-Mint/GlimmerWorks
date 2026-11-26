using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>StructureConditionResource</para>
/// <para>结构放置条件资源。</para>
/// </summary>
/// <remarks>
/// <para>Supported processors: biome, height, spacing, surface.</para>
/// <para>支持的条件处理器：biome（群系）、height（高度）、spacing（间距）、surface（地表）。</para>
/// </remarks>
[UsedImplicitly]
[DataPackResource("structure_condition")]
public class StructureConditionResource : Resource
{
    /// <summary>
    /// <para>Condition processor type.</para>
    /// <para>条件处理器类型。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("processor")]
    public string Processor { get; set; } = "none";

    /// <summary>
    /// <para>Biomes accepted by the biome processor.</para>
    /// <para>群系条件接受的群系。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("biomes")]
    public List<ResourceRef> Biomes { get; set; } = [];

    /// <summary>
    /// <para>Minimum Y percentage, measured as globalY / dimensionMaxY.</para>
    /// <para>最小高度百分比，使用 globalY / 维度最大Y。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("min_percent")]
    public float MinPercent { get; set; }

    /// <summary>
    /// <para>Maximum Y percentage, measured as globalY / dimensionMaxY.</para>
    /// <para>最大高度百分比，使用 globalY / 维度最大Y。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_percent")]
    public float MaxPercent { get; set; } = 1.0F;

    /// <summary>
    /// <para>Spacing used by the spacing processor. Values less than or equal to 0 are treated as 4.</para>
    /// <para>间距条件使用的间距。小于等于 0 时按 4 处理。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("min_spacing")]
    public int MinSpacing { get; set; } = 4;

    /// <summary>
    /// <para>Axis used by the spacing processor: x, y or both.</para>
    /// <para>间距条件使用的轴：x、y 或 both。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("axis")]
    public string Axis { get; set; } = "x";

    /// <summary>
    /// <para>Legacy spacing flag: true means use global Y instead of global X.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("vertical")]
    public bool Vertical { get; set; }

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        foreach (var biome in Biomes)
        {
            biome.SetPackId(selfPackId);
        }
    }
}
