using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>DecoratorResource</para>
/// <para>群系装饰器资源。</para>
/// </summary>
/// <remarks>
/// <para>Supported types: fill, ore, replace, loot_chest.</para>
/// <para>支持的类型：fill（填充）、ore（矿脉）、replace（替换）、loot_chest（战利品箱）。</para>
/// </remarks>
[UsedImplicitly]
[DataPackResource("decorator")]
public class DecoratorResource : Resource
{
    /// <summary>
    /// <para>Decorator type.</para>
    /// <para>装饰器类型。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("type")]
    public string DecoratorType { get; set; } = "fill";

    /// <summary>
    /// <para>Semantic stage. This is only a label; application order follows the biome's decorator list.</para>
    /// <para>语义阶段（terrain_carving / content_placement）。仅作标签，实际顺序由群系装饰器列表决定。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("stage")]
    public string Stage { get; set; } = "terrain_carving";

    /// <summary>
    /// <para>Target layer bit mask (1/2/4).</para>
    /// <para>目标图层位掩码（1/2/4）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("layer")]
    public int Layer { get; set; } = 4;

    /// <summary>
    /// <para>Tile written by fill, ore and loot chest decorators.</para>
    /// <para>填充、矿脉、战利品箱装饰器写入的瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("tile")]
    public ResourceRef Tile { get; set; } = new();

    /// <summary>
    /// <para>Optional alias for the ore tile.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("ore")]
    public ResourceRef Ore { get; set; } = new();

    /// <summary>
    /// <para>Optional alias for the loot-chest tile.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("chest_tile")]
    public ResourceRef ChestTile { get; set; } = new();

    /// <summary>
    /// <para>Resolve the target tile, accepting the type-specific aliases.</para>
    /// </summary>
    public ResourceRef GetEffectiveTile() => DecoratorType.ToLowerInvariant() switch
    {
        "ore" or "1" when !string.IsNullOrEmpty(Ore.ResourceId) => Ore,
        "loot_chest" or "3" when !string.IsNullOrEmpty(ChestTile.ResourceId) => ChestTile,
        _ => Tile
    };

    /// <summary>
    /// <para>Noise seed offset.</para>
    /// <para>噪声种子偏移。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("seed_offset")]
    public int SeedOffset { get; set; }

    /// <summary>
    /// <para>Optional noise resource. A deterministic default fractal noise is used when empty.</para>
    /// <para>可选噪声资源。为空时使用默认的分形噪声。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("noise")]
    public ResourceRef Noise { get; set; } = new();

    /// <summary>
    /// <para>Noise frequency for the built-in deterministic noise.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("noise_frequency")]
    public float NoiseFrequency { get; set; }

    /// <summary>
    /// <para>Minimum normalized generation height.</para>
    /// <para>最小生成高度（归一化）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("min_height")]
    public float MinHeight { get; set; }

    /// <summary>
    /// <para>Maximum normalized generation height.</para>
    /// <para>最大生成高度（归一化）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_height")]
    public float MaxHeight { get; set; } = 0.5F;

    /// <summary>
    /// <para>Minimum noise threshold.</para>
    /// <para>最小噪声阈值。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("min_noise_threshold")]
    public float MinNoiseThreshold { get; set; } = 0.5F;

    /// <summary>
    /// <para>Maximum noise threshold.</para>
    /// <para>最大噪声阈值。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_noise_threshold")]
    public float MaxNoiseThreshold { get; set; } = 0.8F;

    /// <summary>
    /// <para>When true, the threshold moves from min to max as depth increases. Otherwise it moves from max to min.</para>
    /// <para>为 true 时阈值随深度从最小变到最大；否则从最大变到最小。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("chance_increases_with_depth")]
    public bool ChanceIncreasesWithDepth { get; set; } = true;

    /// <summary>
    /// <para>Maximum placements. 0 or less means unlimited.</para>
    /// <para>数量上限。0 或负数表示不限制。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_count")]
    public int MaxCount { get; set; } = 256;

    /// <summary>
    /// <para>Require the cell above the candidate to be air after previous decorators run.</para>
    /// <para>是否要求候选格上方为空气（使用装饰后的引用表）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("require_surface")]
    public bool RequireSurface { get; set; } = true;

    /// <summary>
    /// <para>Noise threshold for loot chests.</para>
    /// <para>战利品箱的生成阈值。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("generation_threshold")]
    public float GenerationThreshold { get; set; } = 0.55F;

    /// <summary>
    /// <para>Replacement rules, evaluated in order. The first rule whose conditions match wins.</para>
    /// <para>替换规则。按顺序求值，第一个所有条件都匹配的规则生效。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("rules")]
    public List<ReplacementRuleResource> Rules { get; set; } = [];

    /// <summary>
    /// <para>Target layer array index (0,1,2) or -1 when invalid.</para>
    /// <para>目标图层数组索引；非法时返回 -1。</para>
    /// </summary>
    public int LayerIndex => Layer switch
    {
        1 => 0,
        2 => 1,
        4 => 2,
        _ => -1
    };

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        ApplyTypeDefaults();
        SetPackId(Tile, selfPackId);
        SetPackId(Ore, selfPackId);
        SetPackId(ChestTile, selfPackId);
        SetPackId(Noise, selfPackId);
        foreach (var rule in Rules)
        {
            SetPackId(rule.SourceTile, selfPackId);
            SetPackId(rule.TargetTile, selfPackId);
        }
    }

    private void ApplyTypeDefaults()
    {
        var type = DecoratorType.ToLowerInvariant();
        if (type is "ore" or "1")
        {
            if (SeedOffset == 0)
            {
                SeedOffset = 1024;
            }

            if (NoiseFrequency <= 0)
            {
                NoiseFrequency = 0.01F;
            }
        }
        else if (type is "loot_chest" or "3")
        {
            if (SeedOffset == 0)
            {
                SeedOffset = 4096;
            }

            if (NoiseFrequency <= 0)
            {
                NoiseFrequency = 0.02F;
            }

            if (MinHeight <= 0 && MaxHeight <= 0.5F)
            {
                MaxHeight = 1.0F;
            }
        }
    }

    private static void SetPackId(ResourceRef resourceRef, string selfPackId)
    {
        if (string.IsNullOrEmpty(resourceRef.ResourceType) && string.IsNullOrEmpty(resourceRef.ResourceId))
        {
            return;
        }

        resourceRef.SetPackId(selfPackId);
    }
}

/// <summary>
/// <para>ReplacementRuleResource</para>
/// <para>替换装饰器的一条规则。</para>
/// </summary>
[UsedImplicitly]
public class ReplacementRuleResource
{
    /// <summary>
    /// <para>Detection offset X, relative to the current cell.</para>
    /// <para>检测点 X 偏移（相对当前格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("offset_x")]
    public int OffsetX { get; set; }

    /// <summary>
    /// <para>Detection offset Y, relative to the current cell.</para>
    /// <para>检测点 Y 偏移（相对当前格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("offset_y")]
    public int OffsetY { get; set; }

    /// <summary>
    /// <para>Original terrain type mask. 0 disables this condition.</para>
    /// <para>原始地形类型掩码。0 表示不检查。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("source_terrain_mask")]
    public int SourceTerrainMask { get; set; }

    /// <summary>
    /// <para>Source tile that must occupy the target layer at the detection point. Empty disables this condition.</para>
    /// <para>检测点目标图层必须是的源瓦片。为空表示不检查。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("source_tile")]
    public ResourceRef SourceTile { get; set; } = new();

    /// <summary>
    /// <para>Tile written at the current cell when this rule matches.</para>
    /// <para>规则匹配时写入当前格的瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("target_tile")]
    public ResourceRef TargetTile { get; set; } = new();
}
