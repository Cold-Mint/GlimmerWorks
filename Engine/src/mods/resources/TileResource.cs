using GlimmerWorks.Engine.Attributes;
using JetBrains.Annotations;
using Tomlyn.Serialization;

namespace GlimmerWorks.Engine.Mods.Resources;

/// <summary>
/// <para>TileResource</para>
/// <para>瓦片资源。描述一种方块/墙体/装饰的静态属性；具体格子上的动态状态由瓦片状态保存。</para>
/// </summary>
/// <remarks>
/// <para>A tile resource is shared by every cell that uses it. Never store per-cell data here.</para>
/// <para>瓦片资源是共享的。任何与具体格子相关的内容都必须放在瓦片状态中。</para>
/// </remarks>
[DataPackResource("tile")]
public class TileResource : Resource
{
    /// <summary>
    /// <para>MaterialList</para>
    /// <para>瓦片的材质列表</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("material_list")]
    public List<ResourceRef> MaterialList { get; set; } = [];

    /// <summary>
    /// <para>Layer bit mask: background wall = 1, background decoration = 2, foreground = 4.</para>
    /// <para>图层位掩码：背景墙 = 1，背景装饰 = 2，地面 = 4。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("layer")]
    public int Layer { get; set; } = 4;

    /// <summary>
    /// <para>Width in cells. Runtime values are clamped to [1,16].</para>
    /// <para>宽度（格）。运行时会夹到 [1,16]。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("width")]
    public int Width { get; set; } = 1;

    /// <summary>
    /// <para>Height in cells. Runtime values are clamped to [1,16].</para>
    /// <para>高度（格）。运行时会夹到 [1,16]。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("height")]
    public int Height { get; set; } = 1;

    /// <summary>
    /// <para>Anchor: top_left, top_center, top_right, center_left, center, center_right, bottom_left, bottom_center, bottom_right, custom.</para>
    /// <para>锚点类型。默认左下角。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("anchor")]
    public string Anchor { get; set; } = "bottom_left";

    /// <summary>
    /// <para>Custom anchor X, relative to the bottom-left corner. Used when anchor = custom.</para>
    /// <para>自定义锚点 X（相对左下角）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("anchor_x")]
    public int AnchorX { get; set; }

    /// <summary>
    /// <para>Custom anchor Y, relative to the bottom-left corner.</para>
    /// <para>自定义锚点 Y（相对左下角）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("anchor_y")]
    public int AnchorY { get; set; }

    /// <summary>
    /// <para>Collision mask: up = 1, right = 2, down = 4, left = 8.</para>
    /// <para>碰撞掩码：上 = 1，右 = 2，下 = 4，左 = 8。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("collision_mask")]
    public int CollisionMask { get; set; }

    /// <summary>
    /// <para>Collision box width in cells.</para>
    /// <para>碰撞箱宽度（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("collision_box_width")]
    public float CollisionBoxWidth { get; set; } = 1.0F;

    /// <summary>
    /// <para>Collision box height in cells.</para>
    /// <para>碰撞箱高度（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("collision_box_height")]
    public float CollisionBoxHeight { get; set; } = 1.0F;

    /// <summary>
    /// <para>Collision box offset X in cells.</para>
    /// <para>碰撞箱偏移 X（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("collision_box_offset_x")]
    public float CollisionBoxOffsetX { get; set; }

    /// <summary>
    /// <para>Collision box offset Y in cells.</para>
    /// <para>碰撞箱偏移 Y（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("collision_box_offset_y")]
    public float CollisionBoxOffsetY { get; set; }

    /// <summary>
    /// <para>Tags used by attachment filtering.</para>
    /// <para>标签。附着时用于过滤支撑瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("tags")]
    public List<string> Tags { get; set; } = [];

    /// <summary>
    /// <para>Whether another tile may overwrite this tile directly.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("replaceable")]
    public bool Replaceable { get; set; } = true;

    /// <summary>
    /// <para>Whether this tile is unbreakable.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("unbreakable")]
    public bool Unbreakable { get; set; }

    /// <summary>
    /// <para>Automatically append derived category tags before world generation.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("auto_derive_tags")]
    public bool AutoDeriveTags { get; set; } = true;

    /// <summary>
    /// <para>Directions this tile actively searches for support. 0 disables attachment. Same bit layout as collision.</para>
    /// <para>探针掩码。本瓦片主动寻找哪些方向的支撑；0 表示不启用附着。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("probe_mask")]
    public int ProbeMask { get; set; }

    /// <summary>
    /// <para>Directions this tile accepts as support. Defaults to every direction.</para>
    /// <para>接收掩码。本瓦片能被哪些方向附着；默认全部允许。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("receive_mask")]
    public int ReceiveMask { get; set; } = 31;

    /// <summary>
    /// <para>Emits a point light on the foreground layer.</para>
    /// <para>是否在地面层发出点光源。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("emits_light")]
    public bool EmitsLight { get; set; }

    [UsedImplicitly] [TomlPropertyName("light_r")]
    public int LightR { get; set; } = 255;

    [UsedImplicitly] [TomlPropertyName("light_g")]
    public int LightG { get; set; } = 255;

    [UsedImplicitly] [TomlPropertyName("light_b")]
    public int LightB { get; set; } = 255;

    [UsedImplicitly] [TomlPropertyName("light_intensity")]
    public float LightIntensity { get; set; } = 1.0F;

    /// <summary>
    /// <para>Point light radius in cells.</para>
    /// <para>点光源半径（格）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("light_radius")]
    public float LightRadius { get; set; } = 8.0F;

    /// <summary>
    /// <para>Backlight mask color. Alpha is the blocking strength. Applies to every layer.</para>
    /// <para>背光遮罩颜色。Alpha 决定挡光强度，作用于所有图层。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("backlight_mask_r")]
    public int BacklightMaskR { get; set; }

    [UsedImplicitly] [TomlPropertyName("backlight_mask_g")]
    public int BacklightMaskG { get; set; }

    [UsedImplicitly] [TomlPropertyName("backlight_mask_b")]
    public int BacklightMaskB { get; set; }

    [UsedImplicitly] [TomlPropertyName("backlight_mask_a")]
    public int BacklightMaskA { get; set; }

    /// <summary>
    /// <para>Side mask color. Alpha is the blocking strength. Applies to the foreground layer only.</para>
    /// <para>侧面遮罩颜色。Alpha 决定挡光强度，只作用于地面层。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("side_mask_r")]
    public int SideMaskR { get; set; }

    [UsedImplicitly] [TomlPropertyName("side_mask_g")]
    public int SideMaskG { get; set; }

    [UsedImplicitly] [TomlPropertyName("side_mask_b")]
    public int SideMaskB { get; set; }

    [UsedImplicitly] [TomlPropertyName("side_mask_a")]
    public int SideMaskA { get; set; }

    /// <summary>
    /// <para>Minimum growth time in ticks. 0 disables growth.</para>
    /// <para>最短生长 tick。0 表示不生长。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_min_ticks")]
    public long GrowthMinTicks { get; set; }

    /// <summary>
    /// <para>Maximum growth time in ticks. Equal to the minimum when not set.</para>
    /// <para>最长生长 tick。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_max_ticks")]
    public long GrowthMaxTicks { get; set; }

    /// <summary>
    /// <para>Resource that replaces this tile when growth completes.</para>
    /// <para>成熟后替换成的瓦片。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_target")]
    public ResourceRef GrowthTarget { get; set; } = new();

    /// <summary>
    /// <para>Whether growth continues while the chunk is unloaded.</para>
    /// <para>是否允许离线生长。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_condition")]
    public string GrowthCondition { get; set; } = "none";

    [UsedImplicitly] [TomlPropertyName("growth_offline")]
    public bool GrowthOffline { get; set; }

    /// <summary>
    /// <para>Destroy the tile after it matures instead of keeping it.</para>
    /// <para>成熟后是否自毁。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_self_destruct")]
    public bool GrowthSelfDestruct { get; set; }

    /// <summary>
    /// <para>Offset X of the grown tile, relative to the original tile.</para>
    /// <para>成熟后生成点的 X 偏移。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_spawn_offset_x")]
    public int GrowthSpawnOffsetX { get; set; }

    /// <summary>
    /// <para>Offset Y of the grown tile, relative to the original tile.</para>
    /// <para>成熟后生成点的 Y 偏移。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("growth_spawn_offset_y")]
    public int GrowthSpawnOffsetY { get; set; }

    /// <summary>
    /// <para>Container capacity in slots. 0 means this tile has no container.</para>
    /// <para>容器容量（槽位）。0 表示不是容器。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("container_capacity")]
    public int ContainerCapacity { get; set; }

    /// <summary>
    /// <para>Maximum stack size per slot.</para>
    /// <para>每格最大堆叠数量。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("max_stack_per_slot")]
    public int MaxStackPerSlot { get; set; } = 64;

    /// <summary>
    /// <para>Loot table rolled when the container is first opened.</para>
    /// <para>首次打开容器时使用的战利品表。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("loot_table")]
    public ResourceRef LootTable { get; set; } = new();

    /// <summary>
    /// <para>Item spawned when the tile is destroyed. Empty means no drops.</para>
    /// <para>破坏时掉落的物品。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("loot_target")]
    public ResourceRef LootTarget { get; set; } = new();

    /// <summary>
    /// <para>Scale the drop count by the tile area.</para>
    /// <para>是否按面积缩放掉落数量。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("loot_scale_by_area")]
    public bool LootScaleByArea { get; set; } = true;

    /// <summary>
    /// <para>Only the centre cell settles drops. Enabled by default for large tiles.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("loot_only_at_center")]
    public bool LootOnlyAtCenter { get; set; } = true;

    /// <summary>
    /// <para>Restrict an emitting tile's point light to the bottom-left origin cell.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("light_origin_only")]
    public bool LightOriginOnly { get; set; }

    /// <summary>
    /// <para>Hardness of one cell.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("unit_hardness")]
    public float UnitHardness { get; set; } = 1.0F;

    /// <summary>
    /// <para>Scale total hardness by width × height.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("auto_hardness_scale")]
    public bool AutoHardnessScale { get; set; } = true;

    /// <summary>
    /// <para>Durability cost of one cell.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("unit_dig_cost")]
    public int UnitDigCost { get; set; } = 1;

    /// <summary>
    /// <para>Charge one unit of durability per covered cell instead of only the centre cell.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("auto_dig_cost_scale")]
    public bool AutoDigCostScale { get; set; } = true;

    /// <summary>
    /// <para>Allow chain mining of adjacent tiles.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("allow_chain_mining")]
    public bool AllowChainMining { get; set; }

    /// <summary>
    /// <para>Whether the tile is considered solid for terrain/collision queries.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("solid")]
    public bool Solid { get; set; } = true;

    /// <summary>
    /// <para>Whether the tile only occupies part of a cell.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("partial")]
    public bool Partial { get; set; }

    /// <summary>
    /// <para>Allow the placement anchor to mirror when the player faces left.</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("allow_dir_adjust_anchor")]
    public bool AllowDirAdjustAnchor { get; set; } = true;

    /// <summary>
    /// <para>Whether this tile is allowed to drop loot.</para>
    /// <para>是否允许掉落。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("allow_loot")]
    public bool AllowLoot { get; set; } = true;

    /// <summary>
    /// <para>Show a blueprint preview when the player holds this tile.</para>
    /// <para>玩家手持时是否显示蓝图。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("show_blueprint")]
    public bool ShowBlueprint { get; set; } = true;

    /// <summary>
    /// <para>Show an occupancy mask when the player holds this tile.</para>
    /// <para>是否显示占用遮罩。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("show_mask")]
    public bool ShowMask { get; set; }

    /// <summary>
    /// <para>Validity color used by the blueprint preview.</para>
    /// <para>蓝图合法颜色（0xRRGGBB）。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("valid_color")]
    public int ValidColor { get; set; } = 0x55FF55;

    /// <summary>
    /// <para>Direction-specific materials. The direction names are up, right, down, left and background.</para>
    /// <para>按方向区分的材质。方向名为 up、right、down、left、background。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("directional_materials")]
    public List<DirectionalMaterialResource> DirectionalMaterials { get; set; } = [];

    /// <summary>
    /// <para>Layer array index (0,1,2) or -1 when the layer value is invalid.</para>
    /// <para>图层数组索引（0,1,2）；图层值非法时返回 -1。</para>
    /// </summary>
    public int LayerIndex => Layer switch
    {
        1 => 0,
        2 => 1,
        4 => 2,
        _ => -1
    };

    /// <summary>
    /// <para>Width clamped to [1,16].</para>
    /// <para>夹到 [1,16] 的宽度。</para>
    /// </summary>
    public int ClampedWidth => Math.Clamp(Width, 1, 16);

    /// <summary>
    /// <para>Height clamped to [1,16].</para>
    /// <para>夹到 [1,16] 的高度。</para>
    /// </summary>
    public int ClampedHeight => Math.Clamp(Height, 1, 16);

    /// <summary>
    /// <para>Validates data that the runtime depends on. Returns false when the resource cannot be used.</para>
    /// <para>校验运行时依赖的数据。返回 false 表示资源不可用。</para>
    /// </summary>
    public bool Validate()
    {
        DeriveTags();
        return LayerIndex >= 0;
    }

    /// <summary>
    /// <para>Append category tags according to the design rules. Air/void identity tiles do not derive wall/decoration.</para>
    /// </summary>
    public void DeriveTags()
    {
        if (!AutoDeriveTags)
        {
            return;
        }

        var isAirOrVoid = HasTag("air") || HasTag("void");
        if (Replaceable)
        {
            AddTag("replaceable");
        }
        else if (Unbreakable)
        {
            AddTag("unbreakable");
        }

        if (Partial)
        {
            AddTag("partial");
        }

        if (Solid)
        {
            AddTag("solid");
        }

        if (!isAirOrVoid)
        {
            if (LayerIndex == 1)
            {
                AddTag("wall");
            }
            else if (LayerIndex == 2)
            {
                AddTag("decoration");
            }
        }
    }

    public bool HasTag(string tag) =>
        Tags.Any(candidate => string.Equals(candidate, tag, StringComparison.OrdinalIgnoreCase));

    private void AddTag(string tag)
    {
        if (!HasTag(tag))
        {
            Tags.Add(tag);
        }
    }

    public override void BeforeRegister(string selfPackId)
    {
        base.BeforeRegister(selfPackId);
        DeriveTags();
        foreach (var material in MaterialList)
        {
            material.SetPackId(selfPackId);
        }

        foreach (var directionalMaterial in DirectionalMaterials)
        {
            directionalMaterial.Material.SetPackId(selfPackId);
        }

        SetPackId(GrowthTarget, selfPackId);
        SetPackId(LootTable, selfPackId);
        SetPackId(LootTarget, selfPackId);
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
/// <para>DirectionalMaterialResource</para>
/// <para>按附着方向区分的材质。</para>
/// </summary>
[UsedImplicitly]
public class DirectionalMaterialResource
{
    /// <summary>
    /// <para>Direction name: up, right, down, left, background.</para>
    /// <para>方向名。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("direction")]
    public string Direction { get; set; } = "right";

    /// <summary>
    /// <para>Material resource.</para>
    /// <para>材质资源。</para>
    /// </summary>
    [UsedImplicitly] [TomlPropertyName("material")]
    public ResourceRef Material { get; set; } = new();
}
