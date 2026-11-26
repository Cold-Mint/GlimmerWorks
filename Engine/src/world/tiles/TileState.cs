using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>TileDestroySource</para>
/// </summary>
public enum TileDestroySource
{
    WorldGeneration,
    Load,
    Unload,
    Player,
    Explosion,
    SupportLost,
    Growth,
    Command
}

/// <summary>
/// <para>TilePlaceSource</para>
/// </summary>
public enum TilePlaceSource
{
    WorldGeneration,
    Structure,
    Decorator,
    Player,
    Growth,
    Load
}

/// <summary>
/// <para>AttachmentDirections</para>
/// </summary>
[Flags]
public enum AttachmentDirections
{
    None = 0,
    Up = 1,
    Right = 2,
    Down = 4,
    Left = 8,
    Background = 16
}

/// <summary>
/// <para>TileState</para>
/// </summary>
/// <remarks>
/// <para><see cref="Offset"/> is always <c>topLeftCell - currentCell</c>.</para>
/// </remarks>
public sealed class TileState
{
    /// <summary>
    /// <para>Resource reference of this cell.</para>
    /// </summary>
    public ResourceRef ResourceRef { get; set; } = new();

    /// <summary>
    /// <para>Tile width in cells.</para>
    /// </summary>
    public int Width { get; set; } = 1;

    /// <summary>
    /// <para>Tile height in cells.</para>
    /// </summary>
    public int Height { get; set; } = 1;

    /// <summary>
    /// <para>Top-left cell minus current cell.</para>
    /// </summary>
    public TileVector2D Offset { get; set; } = new();

    /// <summary>
    /// <para>How this cell was placed.</para>
    /// </summary>
    public TilePlaceSource PlaceSource { get; set; } = TilePlaceSource.WorldGeneration;

    /// <summary>
    /// <para>Resolved attachment direction. Persisted.</para>
    /// </summary>
    public AttachmentDirections AttachmentDirection { get; set; } = AttachmentDirections.None;

    /// <summary>
    /// <para>Unit offset pointing at the support tile: up (0,1), right (1,0), down (0,-1), left (-1,0), background (0,0).</para>
    /// </summary>
    public TileVector2D AttachmentOffset { get; set; } = new();

    /// <summary>
    /// <para>Layer bit of the support tile for background attachments. 0 when not attached.</para>
    /// </summary>
    public int AttachmentLayer { get; set; }

    /// <summary>
    /// <para>Global tick when the tile was placed.</para>
    /// </summary>
    public long TilePlacedTick { get; set; }

    /// <summary>
    /// <para>Global tick when growth started.</para>
    /// </summary>
    public long TileGrowthStartTick { get; set; }

    /// <summary>
    /// <para>Elapsed growth ticks. Serialized as tilegrowthaccumulatedtick.</para>
    /// </summary>
    public long GrowthTicks { get; set; }

    /// <summary>
    /// <para>Growth ticks required to mature. 0 disables growth. Serialized as tilegrowthrequiredtick.</para>
    /// </summary>
    public long GrowthDurationTicks { get; set; }

    /// <summary>
    /// <para>Whether growth has completed.</para>
    /// </summary>
    public bool GrowthMature { get; set; }

    /// <summary>
    /// <para>Number of times this tile has matured.</para>
    /// </summary>
    public int MatureCount { get; set; }

    /// <summary>
    /// <para>Whether growth conditions were met at the last check.</para>
    /// </summary>
    public bool GrowthConditionsMet { get; set; }

    /// <summary>
    /// <para>Whether this state is the bottom-left origin cell.</para>
    /// </summary>
    public bool IsOriginCell => Offset.X == 0 && Offset.Y == Height - 1;

    /// <summary>
    /// <para>Container contents. Null when this cell is not a container.</para>
    /// </summary>
    public ContainerState? Container { get; set; }

    /// <summary>
    /// <para>Loot identity. Only written on the origin cell of a multi-tile tile.</para>
    /// </summary>
    public Guid? Uuid { get; set; }

    /// <summary>
    /// <para>Top-left cell derived from the current cell and <see cref="Offset"/>.</para>
    /// </summary>
    public TileVector2D GetTopLeft(TileVector2D current) =>
        new(current.X + Offset.X, current.Y + Offset.Y);

    /// <summary>
    /// <para>Create a deep copy.</para>
    /// </summary>
    public TileState Clone()
    {
        var resourceRef = new ResourceRef
        {
            TargetPackId = ResourceRef.TargetPackId,
            ResourceId = ResourceRef.ResourceId,
            ResourceType = ResourceRef.ResourceType
        };
        resourceRef.SetPackId(string.IsNullOrEmpty(ResourceRef.RequesterPackId)
            ? ResourceRef.TargetPackId
            : ResourceRef.RequesterPackId);

        return new TileState
        {
            ResourceRef = resourceRef,
            Width = Width,
            Height = Height,
            Offset = new TileVector2D(Offset.X, Offset.Y),
            PlaceSource = PlaceSource,
            AttachmentDirection = AttachmentDirection,
            AttachmentOffset = new TileVector2D(AttachmentOffset.X, AttachmentOffset.Y),
            AttachmentLayer = AttachmentLayer,
            TilePlacedTick = TilePlacedTick,
            TileGrowthStartTick = TileGrowthStartTick,
            GrowthTicks = GrowthTicks,
            GrowthDurationTicks = GrowthDurationTicks,
            GrowthMature = GrowthMature,
            MatureCount = MatureCount,
            GrowthConditionsMet = GrowthConditionsMet,
            Container = Container?.Clone(),
            Uuid = Uuid
        };
    }
}

/// <summary>
/// <para>ContainerState</para>
/// </summary>
public sealed class ContainerState
{
    public int Capacity { get; set; }

    public int MaxStackPerSlot { get; set; } = 64;

    /// <summary>
    /// <para>Whether loot has already been generated for this container.</para>
    /// </summary>
    public bool LootGenerated { get; set; }

    public List<ContainerSlot> Slots { get; set; } = [];

    public void EnsureSlots()
    {
        while (Slots.Count < Capacity)
        {
            Slots.Add(new ContainerSlot());
        }
    }

    public ContainerState Clone()
    {
        return new ContainerState
        {
            Capacity = Capacity,
            MaxStackPerSlot = MaxStackPerSlot,
            LootGenerated = LootGenerated,
            Slots = Slots.Select(static slot => slot.Clone()).ToList()
        };
    }
}

/// <summary>
/// <para>ContainerSlot</para>
/// </summary>
public sealed class ContainerSlot
{
    public ResourceRef? Item { get; set; }

    public int Count { get; set; }

    public bool IsEmpty => Item is null || Count <= 0;

    public ContainerSlot Clone()
    {
        return new ContainerSlot
        {
            Item = Item is null
                ? null
                : new ResourceRef
                {
                    TargetPackId = Item.TargetPackId,
                    ResourceId = Item.ResourceId,
                    ResourceType = Item.ResourceType
                },
            Count = Count
        };
    }
}
