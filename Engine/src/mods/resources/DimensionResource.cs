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
    /// <para>Sea level Y. Columns at or below this height are filled with water.</para>
    /// <para>海平面 Y。低于或等于该高度的空气格会变成水。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("sea_level_y")]
    public int SeaLevelY { get; set; } = 160;

    /// <summary>
    /// <para>Minimum continental height noise output mapped to Y.</para>
    /// <para>大陆高度噪声映射到的最小 Y。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("min_continental_y")]
    public int MinContinentalY { get; set; } = 64;

    /// <summary>
    /// <para>Maximum continental height noise output mapped to Y.</para>
    /// <para>大陆高度噪声映射到的最大 Y。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("max_continental_y")]
    public int MaxContinentalY { get; set; } = 176;

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

    /// <summary>
    /// <para>Tile used for cells outside the dimension bounds (foreground).</para>
    /// <para>维度边界外的地面层瓦片（虚空）。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("void_tile")]
    public ResourceRef VoidTile { get; set; } = new();

    /// <summary>
    /// <para>Tile used for cells outside the dimension bounds (background decoration).</para>
    /// <para>维度边界外的背景装饰层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("void_decoration")]
    public ResourceRef VoidDecoration { get; set; } = new();

    /// <summary>
    /// <para>Tile used for cells outside the dimension bounds (background wall).</para>
    /// <para>维度边界外的背景墙层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("void_wall")]
    public ResourceRef VoidWall { get; set; } = new();

    /// <summary>
    /// <para>Tile used for air cells (foreground). Empty references mean "no tile".</para>
    /// <para>空气格的地面层瓦片。空引用表示该格没有瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("air_tile")]
    public ResourceRef AirTile { get; set; } = new();

    /// <summary>
    /// <para>Tile used for air cells (background decoration).</para>
    /// <para>空气格的背景装饰层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("air_decoration")]
    public ResourceRef AirDecoration { get; set; } = new();

    /// <summary>
    /// <para>Tile used for air cells (background wall).</para>
    /// <para>空气格的背景墙层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("air_wall")]
    public ResourceRef AirWall { get; set; } = new();

    /// <summary>
    /// <para>Water tile (foreground).</para>
    /// <para>水体的地面层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("water_tile")]
    public ResourceRef WaterTile { get; set; } = new();

    /// <summary>
    /// <para>Bedrock tile (foreground and background wall).</para>
    /// <para>基岩的地面层与背景墙层瓦片。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("bedrock_tile")]
    public ResourceRef BedrockTile { get; set; } = new();

    /// <summary>
    /// <para>Backlight color keyframes over normalized time [0,1].</para>
    /// <para>归一化时间 [0,1] 上的背光颜色关键帧。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("backlight_keyframes")]
    public List<LightKeyframeResource> BacklightKeyframes { get; set; } = [];

    /// <summary>
    /// <para>Sky light color keyframes over normalized time [0,1].</para>
    /// <para>归一化时间 [0,1] 上的天光颜色关键帧。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("skylight_keyframes")]
    public List<LightKeyframeResource> SkylightKeyframes { get; set; } = [];

    /// <summary>
    /// <para>Time flow multiplier. 0 disables the day/night cycle.</para>
    /// <para>时间流速。0 表示没有昼夜循环。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("time_flow")]
    public float TimeFlow { get; set; } = 1.0F;

    /// <summary>
    /// <para>Initial normalized time in [0,1].</para>
    /// <para>首次进入维度时的归一化时间。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("initial_time")]
    public float InitialTime { get; set; }

    /// <summary>
    /// <para>Stable identifier of this dimension.</para>
    /// <para>维度的稳定标识。</para>
    /// </summary>
    public string Id => $"{PackId}:{ResourceId}";

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        SetPackId(ElevationNoise, selfPackId);
        SetPackId(HumidityNoise, selfPackId);
        SetPackId(TemperatureNoise, selfPackId);
        SetPackId(WeirdnessNoise, selfPackId);
        SetPackId(ErosionNoise, selfPackId);
        SetPackId(VoidTile, selfPackId);
        SetPackId(VoidDecoration, selfPackId);
        SetPackId(VoidWall, selfPackId);
        SetPackId(AirTile, selfPackId);
        SetPackId(AirDecoration, selfPackId);
        SetPackId(AirWall, selfPackId);
        SetPackId(WaterTile, selfPackId);
        SetPackId(BedrockTile, selfPackId);

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

/// <summary>
/// <para>LightKeyframeResource</para>
/// <para>光照颜色关键帧。</para>
/// </summary>
[UsedImplicitly]
public class LightKeyframeResource
{
    /// <summary>
    /// <para>Normalized time in [0,1].</para>
    /// <para>归一化时间 [0,1]。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("time")]
    public float Time { get; set; }

    [UsedImplicitly] [TomlPropertyName("r")] public int R { get; set; }

    [UsedImplicitly] [TomlPropertyName("g")] public int G { get; set; }

    [UsedImplicitly] [TomlPropertyName("b")] public int B { get; set; }

    [UsedImplicitly] [TomlPropertyName("a")] public int A { get; set; } = 255;
}
