using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>DimensionResource</para>
/// <para>维度资源。描述一套独立的地形边界、噪声配置、群系集合、时间流速和区块存储。</para>
/// </summary>
[UsedImplicitly]
[DataPackResource("dimension")]
public class DimensionResource : Resource
{
    /// <summary>
    /// <para>AllowAsStarting</para>
    /// <para>是否可以作为新世界的起始维度</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("allow_as_starting")]
    public bool AllowAsStarting { get; set; }

    /// <summary>
    /// <para>Minimum world X coordinate (inclusive).</para>
    /// <para>世界最小 X 坐标（包含）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("min_x")]
    public int MinX { get; set; } = -600000;

    /// <summary>
    /// <para>Maximum world X coordinate (inclusive).</para>
    /// <para>世界最大 X 坐标（包含）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("max_x")]
    public int MaxX { get; set; } = 600000;

    /// <summary>
    /// <para>Minimum world Y coordinate (inclusive).</para>
    /// <para>世界最小 Y 坐标（包含）。同时用于海拔归一化。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("min_y")]
    public int MinY { get; set; }

    /// <summary>
    /// <para>Maximum world Y coordinate (inclusive).</para>
    /// <para>世界最大 Y 坐标（包含）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("max_y")]
    public int MaxY { get; set; } = 320;
    

    /// <summary>
    /// <para>Sky region height. Used by sky light and spawn helpers.</para>
    /// <para>天空区域高度。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("sky_height")]
    public int SkyHeight { get; set; } = 16;

    /// <summary>
    /// <para>Continental height noise. Sampled once per column to produce the surface height.</para>
    /// <para>大陆高度噪声。每列采样一次，用于生成地表高度。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("elevation_noise")]
    public ResourceRef ElevationNoise { get; set; } = new();

    /// <summary>
    /// <para>Humidity noise</para>
    /// <para>湿度噪声</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("humidity_noise")]
    public ResourceRef HumidityNoise { get; set; } = new();

    /// <summary>
    /// <para>Temperature noise</para>
    /// <para>温度噪声</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("temperature_noise")]
    public ResourceRef TemperatureNoise { get; set; } = new();

    /// <summary>
    /// <para>Weirdness noise</para>
    /// <para>怪异度噪声</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("weirdness_noise")]
    public ResourceRef WeirdnessNoise { get; set; } = new();

    /// <summary>
    /// <para>Erosion noise</para>
    /// <para>侵蚀度噪声</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("erosion_noise")]
    public ResourceRef ErosionNoise { get; set; } = new();

    /// <summary>
    /// <para>Biomes available in this dimension.</para>
    /// <para>该维度可用的群系列表。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("biomes")]
    public List<ResourceRef> Biomes { get; set; } = [];
    

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        SetPackId(ElevationNoise, selfPackId);
        SetPackId(HumidityNoise, selfPackId);
        SetPackId(TemperatureNoise, selfPackId);
        SetPackId(WeirdnessNoise, selfPackId);
        SetPackId(ErosionNoise, selfPackId);
        foreach (var biome in Biomes)
        {
            biome.SetPackId(selfPackId);
        }
    }

    private static void SetPackId(ResourceRef resourceRef, string selfPackId)
    {
        // An untouched default reference has no resource type or id. Do not mark it as valid.
        if (string.IsNullOrEmpty(resourceRef.ResourceType) && string.IsNullOrEmpty(resourceRef.ResourceId))
        {
            return;
        }

        resourceRef.SetPackId(selfPackId);
    }
}