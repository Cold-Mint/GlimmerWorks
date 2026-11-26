using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>NoiseResource</para>
/// <para>噪声资源。描述一个可在世界中采样的噪声场。</para>
/// </summary>
/// <remarks>
/// <para>The final noise seed is <c>world_seed + seed_offset</c>.</para>
/// <para>实际噪声种子 = 世界种子 + seed_offset。</para>
/// </remarks>
[UsedImplicitly]
[DataPackResource("noise")]
public class NoiseResource : Resource
{
    /// <summary>
    /// <para>SeedOffset</para>
    /// <para>噪声种子偏移</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("seed_offset")]
    public int SeedOffset { get; set; }

    /// <summary>
    /// <para>Fractal octave count. 0 or 1 disables fractal noise.</para>
    /// <para>分形八度数。0 或 1 表示不使用分形。</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("octaves")]
    public int Octaves { get; set; } = 3;

    /// <summary>
    /// <para>Noise frequency</para>
    /// <para>噪声频率</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("frequency")]
    public float Frequency { get; set; } = 0.01F;

    /// <summary>
    /// <para>Fractal lacunarity</para>
    /// <para>分形间隙度</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("lacunarity")]
    public float Lacunarity { get; set; } = 2.0F;

    /// <summary>
    /// <para>Fractal gain</para>
    /// <para>分形增益</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("gain")]
    public float Gain { get; set; } = 0.5F;

    /// <summary>
    /// <para>Fractal weighted strength</para>
    /// <para>分形加权强度</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("weighted_strength")]
    public float WeightedStrength { get; set; }

    /// <summary>
    /// <para>Fractal ping-pong strength</para>
    /// <para>分形乒乓强度</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("ping_pong_strength")]
    public float PingPongStrength { get; set; } = 2.0F;

    /// <summary>
    /// <para>Cellular jitter</para>
    /// <para>细胞抖动</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("cellular_jitter")]
    public float CellularJitter { get; set; } = 1.0F;

    /// <summary>
    /// <para>Noise type: open_simplex2, open_simplex2s, cellular, perlin, value_cubic, value</para>
    /// <para>噪声类型</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("noise_type")]
    public string NoiseType { get; set; } = "perlin";

    /// <summary>
    /// <para>Fractal type: none, fbm, ridged, ping_pong, domain_warp_progressive, domain_warp_independent</para>
    /// <para>分形类型</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("fractal_type")]
    public string FractalType { get; set; } = "none";

    /// <summary>
    /// <para>Cellular distance function: euclidean, euclidean_sq, manhattan, hybrid</para>
    /// <para>细胞距离函数</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("cellular_distance_function")]
    public string CellularDistanceFunction { get; set; } = "euclidean_sq";

    /// <summary>
    /// <para>Cellular return type: cell_value, distance, distance2, distance2_add, distance2_sub, distance2_mul, distance2_div</para>
    /// <para>细胞返回类型</para>
    /// </summary>
    [UsedImplicitly]
    [TomlPropertyName("cellular_return_type")]
    public string CellularReturnType { get; set; } = "distance";
}
