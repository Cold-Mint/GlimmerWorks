using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Systems;

/// <summary>
/// <para>GrowthSystem</para>
/// </summary>
public sealed class GrowthSystem(ChunkStore store)
{
    /// <summary>
    /// <para>Advance growth by the given number of ticks.</para>
    /// </summary>
    /// <summary>
    /// <para>Catch up offline growth after loading a saved chunk.</para>
    /// </summary>
    public void CatchUp(Chunk chunk, long currentTick)
    {
        var actions = new List<(Chunk Chunk, int LayerIndex, int Index, Tile Tile, TileState State)>();
        chunk.ForEachOccupied((layerIndex, index, tile, state) =>
        {
            if (state.GrowthDurationTicks <= 0 || state.GrowthMature || !state.IsOriginCell ||
                !tile.Resource.GrowthOffline)
            {
                return;
            }

            if (state.TileGrowthStartTick > currentTick)
            {
                return;
            }

            var elapsed = currentTick - state.TileGrowthStartTick;
            if (elapsed <= state.GrowthTicks)
            {
                return;
            }

            state.GrowthTicks = Math.Min(elapsed, state.GrowthDurationTicks);
            if (state.GrowthTicks >= state.GrowthDurationTicks)
            {
                actions.Add((chunk, layerIndex, index, tile, state));
            }
        });

        foreach (var action in actions)
        {
            Mature(action.Chunk, action.LayerIndex, action.Index, action.Tile, action.State);
        }
    }

    public void Tick(long ticks)
    {
        if (ticks <= 0)
        {
            return;
        }

        var actions = new List<(Chunk Chunk, int LayerIndex, int Index, Tile Tile, TileState State)>();
        foreach (var chunk in store.GetLoadedChunks())
        {
            chunk.ForEachOccupied((layerIndex, index, tile, state) =>
            {
                if (state.GrowthDurationTicks <= 0 || state.GrowthMature || !state.IsOriginCell)
                {
                    return;
                }

                if (tile.Resource.GrowthMinTicks <= 0)
                {
                    return;
                }

                state.GrowthTicks += ticks;
                if (state.GrowthTicks >= state.GrowthDurationTicks)
                {
                    actions.Add((chunk, layerIndex, index, tile, state));
                }
            });
        }

        foreach (var action in actions)
        {
            Mature(action.Chunk, action.LayerIndex, action.Index, action.Tile, action.State);
        }
    }

    private void Mature(Chunk chunk, int layerIndex, int index, Tile tile, TileState state)
    {
        state.GrowthMature = true;
        state.GrowthTicks = state.GrowthDurationTicks;
        state.MatureCount++;
        tile.OnGrown(chunk, WorldConstants.IndexToLayer(layerIndex), index, chunk.GetWorldCell(index), state);

        var cell = chunk.GetWorldCell(index);
        var resource = tile.Resource;

        if (resource.GrowthSelfDestruct)
        {
            chunk.ClearCell(layerIndex, index, TileDestroySource.Growth);
            return;
        }

        var target = resource.GrowthTarget;
        if (string.IsNullOrEmpty(target.ResourceId))
        {
            return;
        }

        var targetResource = store.Tiles.Find(target);
        if (targetResource is null)
        {
            return;
        }

        var spawnCell = new TileVector2D(cell.X + resource.GrowthSpawnOffsetX,
            cell.Y + resource.GrowthSpawnOffsetY);
        var topLeft = new TileVector2D(spawnCell.X, spawnCell.Y + targetResource.ClampedHeight - 1);
        chunk.World?.PlaceTile(topLeft, WorldConstants.IndexToLayer(layerIndex), target, targetResource,
            TileDestroySource.Growth, TilePlaceSource.Growth);
    }
}

/// <summary>
/// <para>ContainerSystem</para>
/// </summary>
public sealed class ContainerSystem
{
    public ContainerSystem(DataRepository dataRepository)
    {
        DataRepository = dataRepository;
    }

    public DataRepository DataRepository { get; }

    /// <summary>
    /// <para>Open a container and roll loot on first open.</para>
    /// </summary>
    public bool Open(Chunk chunk, int layerIndex, int index, long worldSeed)
    {
        var state = chunk.GetState(layerIndex, index);
        var tile = chunk.GetTile(layerIndex, index);
        if (state?.Container is null || tile is null)
        {
            return false;
        }

        if (state.Container.LootGenerated)
        {
            return true;
        }

        state.Container.LootGenerated = true;
        if (string.IsNullOrEmpty(tile.Resource.LootTable.ResourceId))
        {
            return true;
        }

        var table = DataRepository.Get<LootTableResource>(tile.Resource.LootTable);
        if (table is null)
        {
            return true;
        }

        var seed = WorldHash.Combine(worldSeed, chunk.Position.X, chunk.Position.Y, layerIndex, index);
        var slots = LootRoller.Roll(table, state.Container, seed);
        state.Container.Slots = slots;
        return true;
    }
}

/// <summary>
/// <para>LootRoller</para>
/// </summary>
public static class LootRoller
{
    public static List<ContainerSlot> Roll(LootTableResource table, ContainerState container, ulong seed)
    {
        var random = new DeterministicRandom(seed);
        var rollsMin = Math.Max(0, Math.Min(table.RollsMin, table.RollsMax));
        var rollsMax = Math.Max(rollsMin, table.RollsMax);
        var rolls = rollsMin + random.NextInt(rollsMax - rollsMin + 1);
        var slots = new List<ContainerSlot>(Math.Max(container.Capacity, rolls));
        for (var i = 0; i < container.Capacity; i++)
        {
            slots.Add(new ContainerSlot());
        }

        var totalWeight = table.Entries.Sum(static entry => Math.Max(0.0F, entry.Weight));
        if (totalWeight <= 0)
        {
            return slots;
        }

        for (var roll = 0; roll < rolls && table.Entries.Count > 0; roll++)
        {
            var pick = random.NextFloat() * totalWeight;
            LootEntryResource? chosen = null;
            foreach (var entry in table.Entries)
            {
                pick -= Math.Max(0.0F, entry.Weight);
                if (pick <= 0)
                {
                    chosen = entry;
                    break;
                }
            }

            chosen ??= table.Entries[^1];
            if (string.IsNullOrEmpty(chosen.Item.ResourceId))
            {
                continue;
            }

            var countMin = Math.Max(1, chosen.MinCount);
            var countMax = Math.Max(countMin, chosen.MaxCount);
            var count = countMin + random.NextInt(countMax - countMin + 1);

            var freeSlot = slots.FirstOrDefault(static slot => slot.IsEmpty);
            if (freeSlot is null)
            {
                continue;
            }

            freeSlot.Item = chosen.Item;
            freeSlot.Count = Math.Min(count, container.MaxStackPerSlot);
        }

        return slots;
    }
}

/// <summary>
/// <para>DeterministicRandom</para>
/// </summary>
public sealed class DeterministicRandom(ulong seed)
{
    private ulong _state = seed == 0 ? 0x9E3779B97F4A7C15UL : seed;

    public ulong NextUInt64()
    {
        var z = _state += 0x9E3779B97F4A7C15UL;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9UL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBUL;
        return z ^ (z >> 31);
    }

    public int NextInt(int exclusiveMax) =>
        exclusiveMax <= 0 ? 0 : (int)(NextUInt64() % (ulong)exclusiveMax);

    public float NextFloat() => (NextUInt64() >> 40) / (float)(1UL << 24);
}
