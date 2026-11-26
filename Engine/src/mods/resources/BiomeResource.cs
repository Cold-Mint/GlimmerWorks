using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>BiomeResource</para>
/// <para>群系资源。群系由六个气候目标值、六个严格度系数、所属维度与装饰器列表组成。</para>
/// </summary>
/// <remarks>
/// <para>All climate targets are normalized to [0,1]. A biome whose dimension list is empty is never matched.</para>
/// <para>所有气候目标值都归一化到 [0,1]。所属维度列表为空的群系永远不会被匹配。</para>
/// </remarks>
[UsedImplicitly]
[DataPackResource("biome")]
public class BiomeResource : Resource
{
    /// <summary>
    /// <para>Expected humidity</para>
    /// <para>预期湿度值</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("humidity")]
    public float Humidity { get; set; } = 0.5F;

    /// <summary>
    /// <para>Expected temperature</para>
    /// <para>预期温度值</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("temperature")]
    public float Temperature { get; set; } = 0.5F;

    /// <summary>
    /// <para>Expected weirdness</para>
    /// <para>预期怪异度值</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("weirdness")]
    public float Weirdness { get; set; } = 0.5F;

    /// <summary>
    /// <para>Expected erosion</para>
    /// <para>预期侵蚀度值</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("erosion")]
    public float Erosion { get; set; } = 0.5F;

    /// <summary>
    /// <para>Expected elevation</para>
    /// <para>预期海拔值</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("elevation")]
    public float Elevation { get; set; } = 0.5F;

    /// <summary>
    /// <para>Expected surface proximity</para>
    /// <para>预期地表贴近度</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("surface_proximity")]
    public float SurfaceProximity { get; set; } = 0.5F;

    /// <summary>
    /// <para>Strictness weight for humidity. The default 1.0 is the standard weight.</para>
    /// <para>湿度严格度系数。默认 1.0 表示标准权重。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_humidity")]
    public float StrictnessHumidity { get; set; } = 1.0F;

    /// <summary>
    /// <para>Strictness weight for temperature</para>
    /// <para>温度严格度系数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_temperature")]
    public float StrictnessTemperature { get; set; } = 1.0F;

    /// <summary>
    /// <para>Strictness weight for weirdness</para>
    /// <para>怪异度严格度系数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_weirdness")]
    public float StrictnessWeirdness { get; set; } = 1.0F;

    /// <summary>
    /// <para>Strictness weight for erosion</para>
    /// <para>侵蚀度严格度系数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_erosion")]
    public float StrictnessErosion { get; set; } = 1.0F;

    /// <summary>
    /// <para>Strictness weight for elevation</para>
    /// <para>海拔严格度系数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_elevation")]
    public float StrictnessElevation { get; set; } = 1.0F;

    /// <summary>
    /// <para>Strictness weight for surface proximity</para>
    /// <para>地表贴近度严格度系数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("strictness_surface_proximity")]
    public float StrictnessSurfaceProximity { get; set; } = 1.0F;

    /// <summary>
    /// <para>Dimensions containing this biome. Empty means the biome belongs to no dimension.</para>
    /// <para>所属维度列表。为空表示不属于任何维度。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("dimensions")]
    public List<ResourceRef> Dimensions { get; set; } = [];

    /// <summary>
    /// <para>Decorators applied, in list order, to chunks containing this biome.</para>
    /// <para>装饰器列表。按列表顺序应用。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("decorators")]
    public List<ResourceRef> Decorators { get; set; } = [];

    /// <summary>
    /// <para>Stable identifier of this biome.</para>
    /// <para>群系的稳定标识。</para>
    /// </summary>
    public string Id => $"{PackId}:{ResourceId}";

    /// <summary>
    /// <para>Whether the biome belongs to the given dimension resource reference.</para>
    /// <para>判断群系是否属于给定维度。</para>
    /// </summary>
    public bool BelongsTo(ResourceRef dimension)
    {
        foreach (var candidate in Dimensions)
        {
            if (candidate.TargetPackId == dimension.TargetPackId &&
                candidate.ResourceId == dimension.ResourceId)
            {
                return true;
            }
        }

        return false;
    }

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        foreach (var dimension in Dimensions)
        {
            dimension.SetPackId(selfPackId);
        }

        foreach (var decorator in Decorators)
        {
            decorator.SetPackId(selfPackId);
        }
    }
}
