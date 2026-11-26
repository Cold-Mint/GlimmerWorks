using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.World.Tiles;
using MemoryPack;

namespace GlimmerWorks.Engine.World.Save;

[MemoryPackable]
public sealed partial class Vector2DMessage
{
    public int X { get; set; }

    public int Y { get; set; }
}

[MemoryPackable]
public sealed partial class ResourceRefMessage
{
    public string RequesterPackId { get; set; } = string.Empty;

    public string TargetPackId { get; set; } = string.Empty;

    public string ResourceType { get; set; } = string.Empty;

    public string ResourceId { get; set; } = string.Empty;

    public bool IsEmpty => string.IsNullOrEmpty(ResourceId);
}

/// <summary>
/// <para>map manifest.bin</para>
/// </summary>
[MemoryPackable]
public sealed partial class MapManifestMessage
{
    public const int CurrentSaveFormatVersion = 1;

    public int SaveFormatVersion { get; set; } = CurrentSaveFormatVersion;

    public string Name { get; set; } = string.Empty;

    public string GameVersionName { get; set; } = string.Empty;

    public uint GameVersionNumber { get; set; }

    public long Seed { get; set; }

    public long CreateTime { get; set; }

    public ulong GlobalTickCount { get; set; }

    public uint EntityIdIndex { get; set; }
}

[MemoryPackable]
public sealed partial class VisitedDimensionMessage
{
    public ResourceRefMessage Dimension { get; set; } = new();

    public bool Needautomaticspawn { get; set; }
}

[MemoryPackable]
public sealed partial class ItemSlotMessage
{
    public ResourceRefMessage? Item { get; set; }

    public int Count { get; set; }
}

[MemoryPackable]
public sealed partial class LootBoxSnapshotMessage
{
    public List<ItemSlotMessage> Items { get; set; } = [];
}

[MemoryPackable]
public sealed partial class PlayerMessage
{
    public long LastPlayedTime { get; set; }

    public int PermissionLevel { get; set; }

    public uint CurrentDimensionIndex { get; set; }

    public List<VisitedDimensionMessage> VisitedDimensions { get; set; } = [];

    public EntityItemMessage? Entity { get; set; }

    public Dictionary<string, LootBoxSnapshotMessage> OpenedLootBoxSnapshot { get; set; } = [];
}

/// <summary>
/// <para>Reserved dimension-level manifest. The current save flow does not require it.</para>
/// </summary>
[MemoryPackable]
public sealed partial class DimensionManifestMessage
{
    public int SaveFormatVersion { get; set; } = MapManifestMessage.CurrentSaveFormatVersion;

    public double Time { get; set; }

    public Dictionary<string, string> CustomData { get; set; } = [];
}

[MemoryPackable]
public sealed partial class ItemContainerMessage
{
    public int Capacity { get; set; }

    public int MaxStackPerSlot { get; set; } = 64;

    public bool LootGenerated { get; set; }

    public List<ItemSlotMessage> Slots { get; set; } = [];
}

[MemoryPackable]
public sealed partial class TileStateMessage
{
    public ResourceRefMessage? ResourceRef { get; set; }

    public int Width { get; set; } = 1;

    public int Height { get; set; } = 1;

    public Vector2DMessage Offset { get; set; } = new();

    public int PlaceSource { get; set; }

    public uint AttachmentDirection { get; set; }

    public Vector2DMessage AttachmentOffset { get; set; } = new();

    public uint AttachmentLayer { get; set; }

    public ulong TilePlacedTick { get; set; }

    public ulong TileGrowthStartTick { get; set; }

    public ulong TileGrowthRequiredTick { get; set; }

    public ulong TileGrowthAccumulatedTick { get; set; }

    public uint MatureCount { get; set; }

    public bool TileGrowthConditionsMet { get; set; }

    public string? Uuid { get; set; }

    public ItemContainerMessage? ItemContainer { get; set; }

    public bool IsEmpty => ResourceRef is null || ResourceRef.IsEmpty;
}

[MemoryPackable]
public sealed partial class TileStateArrayMessage
{
    public const int SlotCount = WorldConstants.ChunkCellCount;

    public List<TileStateMessage> TileStateMessages { get; set; } = [];

    public void EnsureSlots()
    {
        while (TileStateMessages.Count < SlotCount)
        {
            TileStateMessages.Add(new TileStateMessage());
        }
    }
}

[MemoryPackable]
public sealed partial class ChunkMessage
{
    public Vector2DMessage Position { get; set; } = new();

    public Dictionary<int, TileStateArrayMessage> TileStates { get; set; } = [];
}

[MemoryPackable]
public sealed partial class GameEntityMessage
{
    public ulong Id { get; set; }
}

[MemoryPackable]
public sealed partial class ComponentMessage
{
    public int Type { get; set; }

    public byte[] Data { get; set; } = [];
}

[MemoryPackable]
public sealed partial class EntityItemMessage
{
    public GameEntityMessage GameEntity { get; set; } = new();

    public ResourceRefMessage ResourceRef { get; set; } = new();

    public List<ComponentMessage> Components { get; set; } = [];
}

[MemoryPackable]
public sealed partial class ChunkEntityMessage
{
    public List<EntityItemMessage> Entities { get; set; } = [];
}

/// <summary>
/// <para>Conversion between runtime DTOs and binary save messages.</para>
/// </summary>
public static class SaveMessageMapper
{
    public static ResourceRefMessage ToMessage(ResourceRef? resourceRef)
    {
        if (resourceRef is null)
        {
            return new ResourceRefMessage();
        }

        return new ResourceRefMessage
        {
            RequesterPackId = resourceRef.RequesterPackId,
            TargetPackId = resourceRef.TargetPackId,
            ResourceType = resourceRef.ResourceType,
            ResourceId = resourceRef.ResourceId
        };
    }

    public static ResourceRef ToResourceRef(ResourceRefMessage? message)
    {
        if (message is null || message.IsEmpty)
        {
            return new ResourceRef();
        }

        var resourceRef = new ResourceRef
        {
            TargetPackId = message.TargetPackId,
            ResourceType = message.ResourceType,
            ResourceId = message.ResourceId
        };
        resourceRef.SetPackId(string.IsNullOrEmpty(message.RequesterPackId)
            ? message.TargetPackId
            : message.RequesterPackId);
        return resourceRef;
    }

    public static ChunkMessage ToChunkMessage(ChunkSaveData data)
    {
        var message = new ChunkMessage
        {
            Position = new Vector2DMessage { X = data.X, Y = data.Y }
        };

        foreach (var layer in data.Layers)
        {
            var array = new TileStateArrayMessage();
            for (var i = 0; i < TileStateArrayMessage.SlotCount; i++)
            {
                var state = i < layer.States.Count ? layer.States[i] : null;
                array.TileStateMessages.Add(ToTileMessage(state));
            }

            message.TileStates[layer.Layer] = array;
        }

        return message;
    }

    public static ChunkSaveData ToChunkSaveData(ChunkMessage message)
    {
        var data = new ChunkSaveData
        {
            X = message.Position.X,
            Y = message.Position.Y
        };

        foreach (var (layerBit, array) in message.TileStates)
        {
            if (WorldConstants.LayerToIndex(layerBit) < 0)
            {
                continue;
            }

            var layer = new ChunkLayerSaveData
            {
                Layer = layerBit,
                States = new List<TileStateSaveData?>(TileStateArrayMessage.SlotCount)
            };
            for (var i = 0; i < TileStateArrayMessage.SlotCount; i++)
            {
                var tile = i < array.TileStateMessages.Count ? array.TileStateMessages[i] : null;
                layer.States.Add(tile is null || tile.IsEmpty ? null : FromTileMessage(tile));
            }

            data.Layers.Add(layer);
        }

        return data;
    }

    public static TileStateMessage ToTileMessage(TileStateSaveData? data)
    {
        if (data is null)
        {
            return new TileStateMessage();
        }

        return new TileStateMessage
        {
            ResourceRef = ToMessage(data.Resource?.ToResourceRef()),
            Width = data.Width,
            Height = data.Height,
            Offset = new Vector2DMessage { X = data.OffsetX, Y = data.OffsetY },
            PlaceSource = data.PlaceSource,
            AttachmentDirection = ToSaveDirection((AttachmentDirections)data.AttachmentDirection),
            AttachmentOffset = new Vector2DMessage { X = data.AttachmentOffsetX, Y = data.AttachmentOffsetY },
            AttachmentLayer = (uint)Math.Max(0, data.AttachmentLayer),
            TilePlacedTick = (ulong)Math.Max(0, data.TilePlacedTick),
            TileGrowthStartTick = (ulong)Math.Max(0, data.TileGrowthStartTick),
            TileGrowthRequiredTick = (ulong)Math.Max(0, data.GrowthDurationTicks),
            TileGrowthAccumulatedTick = (ulong)Math.Max(0, data.GrowthTicks),
            MatureCount = (uint)Math.Max(0, data.MatureCount),
            TileGrowthConditionsMet = data.GrowthConditionsMet,
            Uuid = data.Uuid,
            ItemContainer = ToContainerMessage(data.Container)
        };
    }

    public static TileStateSaveData FromTileMessage(TileStateMessage message)
    {
        return new TileStateSaveData
        {
            Resource = message.ResourceRef is null || message.ResourceRef.IsEmpty
                ? null
                : new ResourceRefSaveData
                {
                    RequesterPackId = message.ResourceRef.RequesterPackId,
                    TargetPackId = message.ResourceRef.TargetPackId,
                    ResourceType = message.ResourceRef.ResourceType,
                    ResourceId = message.ResourceRef.ResourceId
                },
            Width = message.Width,
            Height = message.Height,
            OffsetX = message.Offset.X,
            OffsetY = message.Offset.Y,
            PlaceSource = message.PlaceSource,
            AttachmentDirection = (int)FromSaveDirection(message.AttachmentDirection),
            AttachmentOffsetX = message.AttachmentOffset.X,
            AttachmentOffsetY = message.AttachmentOffset.Y,
            AttachmentLayer = (int)message.AttachmentLayer,
            TilePlacedTick = (long)message.TilePlacedTick,
            TileGrowthStartTick = (long)message.TileGrowthStartTick,
            GrowthDurationTicks = (long)message.TileGrowthRequiredTick,
            GrowthTicks = (long)message.TileGrowthAccumulatedTick,
            GrowthMature = message.TileGrowthAccumulatedTick >= message.TileGrowthRequiredTick &&
                           message.TileGrowthRequiredTick > 0,
            MatureCount = (int)message.MatureCount,
            GrowthConditionsMet = message.TileGrowthConditionsMet,
            Uuid = message.Uuid,
            Container = FromContainerMessage(message.ItemContainer)
        };
    }

    /// <summary>
    /// <para>Legacy save direction values: 1=up, 2=right, 3=down, 4=left, 5=background.</para>
    /// </summary>
    public static uint ToSaveDirection(AttachmentDirections direction) => direction switch
    {
        AttachmentDirections.Up => 1,
        AttachmentDirections.Right => 2,
        AttachmentDirections.Down => 3,
        AttachmentDirections.Left => 4,
        AttachmentDirections.Background => 5,
        _ => 0
    };

    public static AttachmentDirections FromSaveDirection(uint direction) => direction switch
    {
        1 => AttachmentDirections.Up,
        2 => AttachmentDirections.Right,
        3 => AttachmentDirections.Down,
        4 => AttachmentDirections.Left,
        5 => AttachmentDirections.Background,
        // Accept the older bit-mask encoding as well.
        8 => AttachmentDirections.Left,
        16 => AttachmentDirections.Background,
        _ => AttachmentDirections.None
    };

    public static ItemContainerMessage? ToContainerMessage(ContainerSaveData? data)
    {
        if (data is null)
        {
            return null;
        }

        return new ItemContainerMessage
        {
            Capacity = data.Capacity,
            MaxStackPerSlot = data.MaxStackPerSlot,
            LootGenerated = data.LootGenerated,
            Slots = data.Slots.Select(static slot => new ItemSlotMessage
            {
                Item = slot.Item is null ? null : new ResourceRefMessage
                {
                    RequesterPackId = slot.Item.RequesterPackId,
                    TargetPackId = slot.Item.TargetPackId,
                    ResourceType = slot.Item.ResourceType,
                    ResourceId = slot.Item.ResourceId
                },
                Count = slot.Count
            }).ToList()
        };
    }

    public static ContainerSaveData? FromContainerMessage(ItemContainerMessage? message)
    {
        if (message is null)
        {
            return null;
        }

        return new ContainerSaveData
        {
            Capacity = message.Capacity,
            MaxStackPerSlot = message.MaxStackPerSlot,
            LootGenerated = message.LootGenerated,
            Slots = message.Slots.Select(static slot => new ContainerSlotSaveData
            {
                Item = slot.Item is null || slot.Item.IsEmpty
                    ? null
                    : new ResourceRefSaveData
                    {
                        RequesterPackId = slot.Item.RequesterPackId,
                        TargetPackId = slot.Item.TargetPackId,
                        ResourceType = slot.Item.ResourceType,
                        ResourceId = slot.Item.ResourceId
                    },
                Count = slot.Count
            }).ToList()
        };
    }
}
