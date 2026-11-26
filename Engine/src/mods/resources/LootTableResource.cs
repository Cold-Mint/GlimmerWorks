using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>LootTableResource</para>
/// <para>战利品表资源。</para>
/// </summary>
[UsedImplicitly]
[DataPackResource("loot_table")]
public class LootTableResource : Resource
{
    /// <summary>
    /// <para>Minimum number of rolls.</para>
    /// <para>最少抽取次数。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("rolls_min")]
    public int RollsMin { get; set; } = 1;

    /// <summary>
    /// <para>Maximum number of rolls.</para>
    /// <para>最多抽取次数。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("rolls_max")]
    public int RollsMax { get; set; } = 1;

    /// <summary>
    /// <para>Loot entries.</para>
    /// <para>战利品条目。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("entries")]
    public List<LootEntryResource> Entries { get; set; } = [];

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        foreach (var entry in Entries)
        {
            entry.Item.SetPackId(selfPackId);
        }
    }
}

/// <summary>
/// <para>LootEntryResource</para>
/// <para>一条战利品。</para>
/// </summary>
[UsedImplicitly]
public class LootEntryResource
{
    [UsedImplicitly] [TomlPropertyName("item")]
    public ResourceRef Item { get; set; } = new();

    [UsedImplicitly] [TomlPropertyName("min_count")]
    public int MinCount { get; set; } = 1;

    [UsedImplicitly] [TomlPropertyName("max_count")]
    public int MaxCount { get; set; } = 1;

    [UsedImplicitly] [TomlPropertyName("weight")]
    public float Weight { get; set; } = 1.0F;
}
