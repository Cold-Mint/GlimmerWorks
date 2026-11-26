using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>ChunkSaveData</para>
/// </summary>
public sealed class ChunkSaveData
{
    public const int CurrentVersion = 1;

    public int Version { get; set; } = CurrentVersion;

    public int X { get; set; }

    public int Y { get; set; }

    public List<ChunkLayerSaveData> Layers { get; set; } = [];
}

public sealed class ChunkLayerSaveData
{
    /// <summary>
    /// <para>Layer bit (1/2/4).</para>
    /// </summary>
    public int Layer { get; set; }

    /// <summary>
    /// <para>256 state slots in y-major, x-minor order. Null entries are empty cells.</para>
    /// </summary>
    public List<TileStateSaveData?> States { get; set; } = [];
}

public sealed class TileStateSaveData
{
    public ResourceRefSaveData? Resource { get; set; }

    public int Width { get; set; } = 1;

    public int Height { get; set; } = 1;

    public int OffsetX { get; set; }

    public int OffsetY { get; set; }

    public int PlaceSource { get; set; }

    public int AttachmentDirection { get; set; }

    public int AttachmentOffsetX { get; set; }

    public int AttachmentOffsetY { get; set; }

    public int AttachmentLayer { get; set; }

    public long TilePlacedTick { get; set; }

    public long TileGrowthStartTick { get; set; }

    public long GrowthTicks { get; set; }

    public long GrowthDurationTicks { get; set; }

    public bool GrowthMature { get; set; }

    public int MatureCount { get; set; }

    public bool GrowthConditionsMet { get; set; }

    public string? Uuid { get; set; }

    public ContainerSaveData? Container { get; set; }
}

public sealed class ContainerSaveData
{
    public int Capacity { get; set; }

    public int MaxStackPerSlot { get; set; } = 64;

    public bool LootGenerated { get; set; }

    public List<ContainerSlotSaveData> Slots { get; set; } = [];
}

public sealed class ContainerSlotSaveData
{
    public ResourceRefSaveData? Item { get; set; }

    public int Count { get; set; }
}

/// <summary>
/// <para>ResourceRefSaveData</para>
/// </summary>
public sealed class ResourceRefSaveData
{
    public string RequesterPackId { get; set; } = string.Empty;

    public string TargetPackId { get; set; } = string.Empty;

    public string ResourceId { get; set; } = string.Empty;

    public string ResourceType { get; set; } = string.Empty;

    public static ResourceRefSaveData From(ResourceRef resourceRef) => new()
    {
        RequesterPackId = resourceRef.RequesterPackId,
        TargetPackId = resourceRef.TargetPackId,
        ResourceId = resourceRef.ResourceId,
        ResourceType = resourceRef.ResourceType
    };

    public ResourceRef ToResourceRef()
    {
        var resourceRef = new ResourceRef
        {
            TargetPackId = TargetPackId,
            ResourceId = ResourceId,
            ResourceType = ResourceType
        };
        resourceRef.SetPackId(string.IsNullOrEmpty(RequesterPackId)
            ? (string.IsNullOrEmpty(TargetPackId) ? ResourceRef.CorePackId : TargetPackId)
            : RequesterPackId);
        return resourceRef;
    }
}

/// <summary>
/// <para>TileStateSerialization</para>
/// </summary>
public static class TileStateSerialization
{
    public static TileStateSaveData ToSaveData(TileState state)
    {
        return new TileStateSaveData
        {
            Resource = ResourceRefSaveData.From(state.ResourceRef),
            Width = state.Width,
            Height = state.Height,
            OffsetX = state.Offset.X,
            OffsetY = state.Offset.Y,
            PlaceSource = (int)state.PlaceSource,
            AttachmentDirection = (int)state.AttachmentDirection,
            AttachmentOffsetX = state.AttachmentOffset.X,
            AttachmentOffsetY = state.AttachmentOffset.Y,
            AttachmentLayer = state.AttachmentLayer,
            TilePlacedTick = state.TilePlacedTick,
            TileGrowthStartTick = state.TileGrowthStartTick,
            GrowthTicks = state.GrowthTicks,
            GrowthDurationTicks = state.GrowthDurationTicks,
            GrowthMature = state.GrowthMature,
            MatureCount = state.MatureCount,
            GrowthConditionsMet = state.GrowthConditionsMet,
            Uuid = state.Uuid?.ToString(),
            Container = state.Container is null
                ? null
                : new ContainerSaveData
                {
                    Capacity = state.Container.Capacity,
                    MaxStackPerSlot = state.Container.MaxStackPerSlot,
                    LootGenerated = state.Container.LootGenerated,
                    Slots = state.Container.Slots.Select(static slot => new ContainerSlotSaveData
                    {
                        Item = slot.Item is null ? null : ResourceRefSaveData.From(slot.Item),
                        Count = slot.Count
                    }).ToList()
                }
        };
    }

    public static TileState FromSaveData(TileStateSaveData data)
    {
        return new TileState
        {
            ResourceRef = data.Resource?.ToResourceRef() ?? new ResourceRef(),
            Width = Math.Clamp(data.Width, 1, WorldConstants.ChunkSize),
            Height = Math.Clamp(data.Height, 1, WorldConstants.ChunkSize),
            Offset = new TileVector2D(data.OffsetX, data.OffsetY),
            PlaceSource = Enum.IsDefined(typeof(TilePlaceSource), data.PlaceSource)
                ? (TilePlaceSource)data.PlaceSource
                : TilePlaceSource.Load,
            AttachmentDirection = Enum.IsDefined(typeof(AttachmentDirections), data.AttachmentDirection)
                ? (AttachmentDirections)data.AttachmentDirection
                : AttachmentDirections.None,
            AttachmentOffset = new TileVector2D(data.AttachmentOffsetX, data.AttachmentOffsetY),
            AttachmentLayer = data.AttachmentLayer,
            TilePlacedTick = data.TilePlacedTick,
            TileGrowthStartTick = data.TileGrowthStartTick,
            GrowthTicks = data.GrowthTicks,
            GrowthDurationTicks = data.GrowthDurationTicks,
            GrowthMature = data.GrowthMature,
            MatureCount = data.MatureCount,
            GrowthConditionsMet = data.GrowthConditionsMet,
            Uuid = Guid.TryParse(data.Uuid, out var uuid) ? uuid : null,
            Container = data.Container is null
                ? null
                : new ContainerState
                {
                    Capacity = data.Container.Capacity,
                    MaxStackPerSlot = data.Container.MaxStackPerSlot,
                    LootGenerated = data.Container.LootGenerated,
                    Slots = data.Container.Slots.Select(static slot => new ContainerSlot
                    {
                        Item = slot.Item?.ToResourceRef(),
                        Count = slot.Count
                    }).ToList()
                }
        };
    }
}
