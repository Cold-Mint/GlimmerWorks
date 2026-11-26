using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>Tile</para>
/// </summary>
public class Tile
{
    public Tile(TileResource resource, ulong fingerprint)
    {
        Resource = resource;
        Fingerprint = fingerprint;
    }

    /// <summary>
    /// <para>Static resource behind this tile.</para>
    /// </summary>
    public TileResource Resource { get; }

    /// <summary>
    /// <para>Resource fingerprint.</para>
    /// </summary>
    public ulong Fingerprint { get; }

    /// <summary>
    /// <para>Called after this instance is installed on a cell.</para>
    /// </summary>
    public virtual void OnPlaced(Chunk chunk, int layer, int index, TileVector2D worldCell, TileState state,
        TilePlaceSource source)
    {
    }

    /// <summary>
    /// <para>Called before this instance is removed or replaced on a cell.</para>
    /// </summary>
    public virtual void OnReplaced(Chunk chunk, int layer, int index, TileVector2D worldCell, TileState state,
        TileDestroySource source)
    {
        // The tile itself decides whether a destroy source drops loot. World generation,
        // loading, unloading and growth replacement never drop.
        if (source is TileDestroySource.Player or TileDestroySource.Explosion
            or TileDestroySource.SupportLost or TileDestroySource.Command)
        {
            chunk.World?.SettleTileLoot(chunk, layer, index, state, this);
        }
    }

    /// <summary>
    /// <para>Called when the owning chunk unloads. Custom tiles must clear external bindings here.</para>
    /// </summary>
    public virtual void OnUnloaded(Chunk chunk, int layer, int index, TileVector2D worldCell, TileState state)
    {
    }

    /// <summary>
    /// <para>Called by the growth system when this tile matures.</para>
    /// </summary>
    public virtual void OnGrown(Chunk chunk, int layer, int index, TileVector2D worldCell, TileState state)
    {
    }
}

/// <summary>
/// <para>TileSnapshot</para>
/// </summary>
public readonly record struct TileSnapshot(Tile Tile, TileState State);

/// <summary>
/// <para>ITileRegistry</para>
/// </summary>
public interface ITileRegistry
{
    /// <summary>
    /// <para>Find a tile resource by reference. Returns null when the reference is invalid or missing.</para>
    /// </summary>
    TileResource? Find(ResourceRef resourceRef);

    /// <summary>
    /// <para>Create a new tile instance for a resource.</para>
    /// </summary>
    Tile CreateInstance(TileResource resource, ulong fingerprint);
}

/// <summary>
/// <para>DataRepositoryTileRegistry</para>
/// </summary>
public sealed class DataRepositoryTileRegistry(
    DataRepository dataRepository,
    Func<TileResource, ulong, Tile>? factory = null)
    : ITileRegistry
{
    private readonly Func<TileResource, ulong, Tile> _factory =
        factory ?? (static (resource, fingerprint) => new Tile(resource, fingerprint));

    public TileResource? Find(ResourceRef resourceRef)
    {
        return dataRepository.Get<TileResource>(resourceRef);
    }

    public Tile CreateInstance(TileResource resource, ulong fingerprint)
    {
        return _factory(resource, fingerprint);
    }
}

/// <summary>
/// <para>TileInstancePool</para>
/// </summary>
public sealed class TileInstancePool
{
    private readonly Dictionary<ulong, WeakReference<Tile>> _tiles = [];
    private readonly object _lock = new();
    private readonly ITileRegistry _registry;

    public TileInstancePool(ITileRegistry registry)
    {
        _registry = registry;
    }

    /// <summary>
    /// <para>Get a shared instance for the fingerprint, creating it when necessary.</para>
    /// </summary>
    public Tile? GetOrCreate(ResourceRef resourceRef, TileResource resource, ulong fingerprint)
    {
        lock (_lock)
        {
            if (_tiles.TryGetValue(fingerprint, out var weak) && weak.TryGetTarget(out var existing))
            {
                return existing;
            }

            var created = _registry.CreateInstance(resource, fingerprint);
            _tiles[fingerprint] = new WeakReference<Tile>(created);
            return created;
        }
    }

    /// <summary>
    /// <para>Remove dead entries from the pool.</para>
    /// </summary>
    public void Collect()
    {
        lock (_lock)
        {
            var dead = new List<ulong>();
            foreach (var (key, value) in _tiles)
            {
                if (!value.TryGetTarget(out _))
                {
                    dead.Add(key);
                }
            }

            foreach (var key in dead)
            {
                _tiles.Remove(key);
            }
        }
    }

    /// <summary>
    /// <para>Number of live or not-yet-collected entries.</para>
    /// </summary>
    public int Count
    {
        get
        {
            lock (_lock)
            {
                return _tiles.Count;
            }
        }
    }
}
