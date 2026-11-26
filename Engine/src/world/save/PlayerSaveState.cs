using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Save;

/// <summary>
/// <para>PlayerPermissionLevel</para>
/// </summary>
public enum PlayerPermissionLevel
{
    Normal = 0,
    Administrator = 1
}

/// <summary>
/// <para>VisitedDimensionEntry</para>
/// <para>玩家访问过的维度。Needautomaticspawn 为预留语义。</para>
/// </summary>
public sealed class VisitedDimensionEntry
{
    public ResourceRef Dimension { get; set; } = new();

    public bool Needautomaticspawn { get; set; }
}

/// <summary>
/// <para>LootBoxSnapshot</para>
/// <para>某个玩家首次打开某个战利品箱时掷出的物品快照。</para>
/// </summary>
public sealed class LootBoxSnapshot
{
    public List<ContainerSlot> Items { get; set; } = [];
}

/// <summary>
/// <para>PlayerSaveState</para>
/// <para>本地玩家的运行时存档状态。</para>
/// </summary>
public sealed class PlayerSaveState
{
    public long LastPlayedTime { get; set; }

    public PlayerPermissionLevel PermissionLevel { get; set; }

    public int CurrentDimensionIndex { get; set; }

    public List<VisitedDimensionEntry> VisitedDimensions { get; set; } = [];

    public EntityItemMessage? Entity { get; set; }

    public Dictionary<Guid, LootBoxSnapshot> OpenedLootBoxSnapshot { get; set; } = [];

    public static PlayerSaveState CreateDefault(ResourceRef dimension, bool allowCheats)
    {
        var state = new PlayerSaveState
        {
            LastPlayedTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            PermissionLevel = allowCheats ? PlayerPermissionLevel.Administrator : PlayerPermissionLevel.Normal
        };
        state.VisitedDimensions.Add(new VisitedDimensionEntry
        {
            Dimension = Clone(dimension),
            Needautomaticspawn = true
        });
        state.CurrentDimensionIndex = 0;
        return state;
    }

    /// <summary>
    /// <para>Record a dimension visit. Existing entries only move the current index.</para>
    /// </summary>
    public void Visit(ResourceRef dimension)
    {
        for (var i = 0; i < VisitedDimensions.Count; i++)
        {
            var entry = VisitedDimensions[i];
            if (Same(entry.Dimension, dimension))
            {
                CurrentDimensionIndex = i;
                return;
            }
        }

        VisitedDimensions.Add(new VisitedDimensionEntry
        {
            Dimension = Clone(dimension),
            Needautomaticspawn = true
        });
        CurrentDimensionIndex = VisitedDimensions.Count - 1;
    }

    public ResourceRef? GetCurrentDimension()
    {
        if (CurrentDimensionIndex < 0 || CurrentDimensionIndex >= VisitedDimensions.Count)
        {
            return null;
        }

        return VisitedDimensions[CurrentDimensionIndex].Dimension;
    }

    public PlayerMessage ToMessage()
    {
        return new PlayerMessage
        {
            LastPlayedTime = LastPlayedTime,
            PermissionLevel = (int)PermissionLevel,
            CurrentDimensionIndex = (uint)Math.Max(0, CurrentDimensionIndex),
            VisitedDimensions = VisitedDimensions.Select(static entry => new VisitedDimensionMessage
            {
                Dimension = SaveMessageMapper.ToMessage(entry.Dimension),
                Needautomaticspawn = entry.Needautomaticspawn
            }).ToList(),
            Entity = Entity,
            OpenedLootBoxSnapshot = OpenedLootBoxSnapshot.ToDictionary(
                static pair => pair.Key.ToString(),
                static pair => new LootBoxSnapshotMessage
                {
                    Items = pair.Value.Items.Select(static slot => new ItemSlotMessage
                    {
                        Item = slot.Item is null ? null : SaveMessageMapper.ToMessage(slot.Item),
                        Count = slot.Count
                    }).ToList()
                })
        };
    }

    public static PlayerSaveState FromMessage(PlayerMessage message)
    {
        var state = new PlayerSaveState
        {
            LastPlayedTime = message.LastPlayedTime,
            PermissionLevel = message.PermissionLevel == (int)PlayerPermissionLevel.Administrator
                ? PlayerPermissionLevel.Administrator
                : PlayerPermissionLevel.Normal,
            CurrentDimensionIndex = (int)message.CurrentDimensionIndex,
            Entity = message.Entity,
            OpenedLootBoxSnapshot = message.OpenedLootBoxSnapshot.ToDictionary(
                static pair => Guid.TryParse(pair.Key, out var uuid) ? uuid : Guid.Empty,
                static pair => new LootBoxSnapshot
                {
                    Items = pair.Value.Items.Select(static slot => new ContainerSlot
                    {
                        Item = slot.Item is null || slot.Item.IsEmpty
                            ? null
                            : SaveMessageMapper.ToResourceRef(slot.Item),
                        Count = slot.Count
                    }).ToList()
                })
        };

        foreach (var visited in message.VisitedDimensions)
        {
            state.VisitedDimensions.Add(new VisitedDimensionEntry
            {
                Dimension = SaveMessageMapper.ToResourceRef(visited.Dimension),
                Needautomaticspawn = visited.Needautomaticspawn
            });
        }

        if (state.CurrentDimensionIndex < 0 || state.CurrentDimensionIndex >= state.VisitedDimensions.Count)
        {
            state.CurrentDimensionIndex = 0;
        }

        return state;
    }

    private static bool Same(ResourceRef left, ResourceRef right) =>
        left.TargetPackId == right.TargetPackId && left.ResourceId == right.ResourceId;

    private static ResourceRef Clone(ResourceRef source)
    {
        var clone = new ResourceRef
        {
            TargetPackId = source.TargetPackId,
            ResourceType = source.ResourceType,
            ResourceId = source.ResourceId
        };
        clone.SetPackId(string.IsNullOrEmpty(source.RequesterPackId)
            ? source.TargetPackId
            : source.RequesterPackId);
        return clone;
    }
}
