using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>StructureResource</para>
/// <para>结构资源。结构由条件列表与结构生成器组成。</para>
/// </summary>
/// <remarks>
/// <para>Supported generator types: static, tree.</para>
/// <para>支持的结构生成器类型：static（静态结构）、tree（树结构）。</para>
/// </remarks>
[UsedImplicitly]
[DataPackResource("structure")]
public class StructureResource : Resource
{
    /// <summary>
    /// <para>Generator type.</para>
    /// <para>结构生成器类型。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("type")]
    public string StructureType { get; set; } = "none";

    /// <summary>
    /// <para>Placement priority. Higher values are evaluated first.</para>
    /// <para>放置优先级。数值越大越先放置。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("priority")]
    public int Priority { get; set; }

    /// <summary>
    /// <para>Placement conditions. An empty list means the structure is never placed.</para>
    /// <para>放置条件列表。为空表示永远不会放置。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("conditions")]
    public List<ResourceRef> Conditions { get; set; } = [];

    /// <summary>
    /// <para>Static structure tiles. Positions are local offsets from the structure origin.</para>
    /// <para>静态结构的瓦片列表。位置是相对结构原点的局部偏移。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("tiles")]
    public List<StaticStructureTileResource> Tiles { get; set; } = [];

    /// <summary>
    /// <para>Generic data references used by the tree generator.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("data")]
    public List<ResourceRef> Data { get; set; } = [];

    /// <summary>
    /// <para>Index of the trunk resource in <see cref="Data"/>.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("trunk_data_index")]
    public int TrunkDataIndex { get; set; }

    /// <summary>
    /// <para>Index of the leaves resource in <see cref="Data"/>.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaves_data_index")]
    public int LeavesDataIndex { get; set; }

    /// <summary>
    /// <para>Resolve the trunk reference from direct data or the indexed data list.</para>
    /// </summary>
    public ResourceRef GetTrunkTile() =>
        !string.IsNullOrEmpty(TrunkTile.ResourceId)
            ? TrunkTile
            : TrunkDataIndex >= 0 && TrunkDataIndex < Data.Count ? Data[TrunkDataIndex] : TrunkTile;

    /// <summary>
    /// <para>Resolve the leaves reference from direct data or the indexed data list.</para>
    /// </summary>
    public ResourceRef GetLeavesTile() =>
        !string.IsNullOrEmpty(LeavesTile.ResourceId)
            ? LeavesTile
            : LeavesDataIndex >= 0 && LeavesDataIndex < Data.Count ? Data[LeavesDataIndex] : LeavesTile;

    /// <summary>
    /// <para>Tree trunk tile.</para>
    /// <para>树干瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("trunk_tile")]
    public ResourceRef TrunkTile { get; set; } = new();

    /// <summary>
    /// <para>Tree leaves tile.</para>
    /// <para>树叶瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaves_tile")]
    public ResourceRef LeavesTile { get; set; } = new();

    /// <summary>
    /// <para>Minimum trunk height.</para>
    /// <para>树干最小高度。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("min_trunk_height")]
    public int MinTrunkHeight { get; set; } = 5;

    /// <summary>
    /// <para>Maximum trunk height.</para>
    /// <para>树干最大高度。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_trunk_height")]
    public int MaxTrunkHeight { get; set; } = 9;

    /// <summary>
    /// <para>Trunk width in cells.</para>
    /// <para>树干宽度（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("trunk_width")]
    public int TrunkWidth { get; set; } = 1;

    /// <summary>
    /// <para>Whether leaves are generated.</para>
    /// <para>是否生成树叶。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("has_leaves")]
    public bool HasLeaves { get; set; }

    /// <summary>
    /// <para>Leaf cluster radius.</para>
    /// <para>叶簇半径。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaf_radius")]
    public int LeafRadius { get; set; } = 2;

    /// <summary>
    /// <para>Number of leaf clusters.</para>
    /// <para>叶簇数量。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaf_clusters")]
    public int LeafClusters { get; set; } = 1;

    /// <summary>
    /// <para>Vertical spacing between leaf clusters.</para>
    /// <para>叶簇之间的垂直间距。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaf_vertical_spacing")]
    public int LeafVerticalSpacing { get; set; }

    /// <summary>
    /// <para>Trunk layer bit mask.</para>
    /// <para>树干图层位掩码。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("trunk_layer")]
    public int TrunkLayer { get; set; }

    /// <summary>
    /// <para>Leaves layer bit mask.</para>
    /// <para>树叶图层位掩码。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("leaves_layer")]
    public int LeavesLayer { get; set; }

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        foreach (var condition in Conditions)
        {
            condition.SetPackId(selfPackId);
        }

        foreach (var tile in Tiles)
        {
            tile.Tile.SetPackId(selfPackId);
        }

        foreach (var data in Data)
        {
            data.SetPackId(selfPackId);
        }

        SetPackId(TrunkTile, selfPackId);
        SetPackId(LeavesTile, selfPackId);
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
/// <para>StaticStructureTileResource</para>
/// <para>静态结构中的一个瓦片。</para>
/// </summary>
[UsedImplicitly]
public class StaticStructureTileResource
{
    [UsedImplicitly] [TomlPropertyName("tile")]
    public ResourceRef Tile { get; set; } = new();

    [UsedImplicitly] [TomlPropertyName("x")]
    public int X { get; set; }

    [UsedImplicitly] [TomlPropertyName("y")]
    public int Y { get; set; }

    /// <summary>
    /// <para>Layer bit mask (1/2/4).</para>
    /// <para>图层位掩码（1/2/4）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("layer")]
    public int Layer { get; set; }
}
