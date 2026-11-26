using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Streaming;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Render;

/// <summary>
/// <para>ResolvedTileTexture</para>
/// </summary>
public readonly record struct ResolvedTileTexture(ResourceRef? Material, bool FlipX, string Direction);

/// <summary>
/// <para>TileDrawCommand</para>
/// </summary>
public readonly record struct TileDrawCommand(
    Tile Tile,
    TileState State,
    TileVector2D TopLeftCell,
    WorldRect WorldBounds,
    int Layer,
    ResolvedTileTexture Texture);

/// <summary>
/// <para>TileTextureResolver</para>
/// </summary>
public static class TileTextureResolver
{
    public static ResolvedTileTexture Resolve(TileResource resource, AttachmentDirections direction)
    {
        var name = direction switch
        {
            AttachmentDirections.Up => "up",
            AttachmentDirections.Right => "right",
            AttachmentDirections.Down => "down",
            AttachmentDirections.Left => "left",
            AttachmentDirections.Background => "background",
            _ => "right"
        };

        var explicitMaterial = FindMaterial(resource, name);
        if (explicitMaterial is not null)
        {
            return new ResolvedTileTexture(explicitMaterial, false, name);
        }

        var defaultMaterial = resource.MaterialList.FirstOrDefault(static material =>
            !string.IsNullOrEmpty(material.ResourceId));

        if (direction == AttachmentDirections.Left)
        {
            // Textures are authored facing right. Mirror the right-facing/default texture.
            var rightMaterial = FindMaterial(resource, "right") ?? defaultMaterial;
            return new ResolvedTileTexture(rightMaterial, true, name);
        }

        return new ResolvedTileTexture(defaultMaterial, false, name);
    }

    private static ResourceRef? FindMaterial(TileResource resource, string direction)
    {
        foreach (var directional in resource.DirectionalMaterials)
        {
            if (string.Equals(directional.Direction, direction, StringComparison.OrdinalIgnoreCase) &&
                !string.IsNullOrEmpty(directional.Material.ResourceId))
            {
                return directional.Material;
            }
        }

        return null;
    }
}

/// <summary>
/// <para>ChunkRenderList</para>
/// </summary>
public sealed class ChunkRenderList
{
    /// <summary>
    /// <para>Collect visible tile draw commands.</para>
    /// </summary>
    public List<TileDrawCommand> Collect(ChunkStore store, WorldRect viewport, int layerMask = 7)
    {
        var commands = new List<TileDrawCommand>();
        var seen = new HashSet<(int X, int Y, int Layer)>();

        var startTile = WorldConstants.WorldToTile(new WorldVector2D(viewport.Left, viewport.Bottom));
        var endTile = WorldConstants.WorldToTile(new WorldVector2D(viewport.Right, viewport.Top));
        var startChunk = WorldConstants.GetChunkVertex(startTile);
        var endChunk = WorldConstants.GetChunkVertex(endTile);

        var layers = new List<int>();
        foreach (var layer in WorldConstants.LayerBits)
        {
            if ((layerMask & layer) != 0)
            {
                layers.Add(layer);
            }
        }

        for (var chunkY = startChunk.Y; chunkY <= endChunk.Y; chunkY += WorldConstants.ChunkSize)
        {
            for (var chunkX = startChunk.X; chunkX <= endChunk.X; chunkX += WorldConstants.ChunkSize)
            {
                var chunk = store.GetChunk(new ChunkVertexVector2D(chunkX, chunkY));
                if (chunk is null)
                {
                    continue;
                }

                foreach (var layer in layers)
                {
                    var layerIndex = WorldConstants.LayerToIndex(layer);
                    var states = chunk.EnumerateLayer(layerIndex).ToList();
                    foreach (var (index, state) in states)
                    {
                        if (state is null)
                        {
                            continue;
                        }

                        var snapshot = chunk.GetSnapshot(layerIndex, index);
                        if (snapshot is null)
                        {
                            continue;
                        }

                        var current = chunk.GetWorldCell(index);
                        var topLeft = state.GetTopLeft(current);
                        if (!seen.Add((topLeft.X, topLeft.Y, layer)))
                        {
                            continue;
                        }

                        var width = Math.Clamp(state.Width, 1, 16);
                        var height = Math.Clamp(state.Height, 1, 16);
                        var bounds = new WorldRect(
                            topLeft.X * (float)WorldConstants.TilePixelSize,
                            (topLeft.Y - height + 1) * (float)WorldConstants.TilePixelSize,
                            (topLeft.X + width) * (float)WorldConstants.TilePixelSize,
                            (topLeft.Y + 1) * (float)WorldConstants.TilePixelSize);

                        if (bounds.Right < viewport.Left || bounds.Left > viewport.Right ||
                            bounds.Top < viewport.Bottom || bounds.Bottom > viewport.Top)
                        {
                            continue;
                        }

                        var texture = TileTextureResolver.Resolve(snapshot.Value.Tile.Resource,
                            state.AttachmentDirection);
                        commands.Add(new TileDrawCommand(snapshot.Value.Tile, state, topLeft, bounds, layer,
                            texture));
                    }
                }
            }
        }

        commands.Sort(static (left, right) =>
        {
            var layer = left.Layer.CompareTo(right.Layer);
            if (layer != 0)
            {
                return layer;
            }

            var y = left.WorldBounds.Bottom.CompareTo(right.WorldBounds.Bottom);
            return y != 0 ? y : left.WorldBounds.Left.CompareTo(right.WorldBounds.Left);
        });

        return commands;
    }
}

/// <summary>
/// <para>ChunkRenderCache</para>
/// </summary>
public sealed class ChunkRenderCache(ChunkRenderList renderList)
{
    private readonly ChunkRenderList _renderList = renderList;
    private ChunkStore? _lastStore;
    private WorldRect _lastViewport;
    private int _lastLayerMask;
    private long _lastRevision = -1;
    private List<TileDrawCommand> _lastCommands = [];

    public IReadOnlyList<TileDrawCommand> Get(ChunkStore store, WorldRect viewport, int layerMask = 7)
    {
        if (ReferenceEquals(_lastStore, store) && _lastRevision == store.Revision &&
            _lastViewport == viewport && _lastLayerMask == layerMask)
        {
            return _lastCommands;
        }

        _lastCommands = _renderList.Collect(store, viewport, layerMask);
        _lastStore = store;
        _lastRevision = store.Revision;
        _lastViewport = viewport;
        _lastLayerMask = layerMask;
        return _lastCommands;
    }
}
