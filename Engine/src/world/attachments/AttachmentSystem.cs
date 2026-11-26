using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods.Resources;
using GlimmerWorks.Engine.World.Storage;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Attachments;

/// <summary>
/// <para>AttachmentSearchResult</para>
/// </summary>
public readonly record struct AttachmentSearchResult(
    bool Found,
    AttachmentDirections Direction,
    TileVector2D Offset,
    int SupportLayer);

/// <summary>
/// <para>AttachmentSystem</para>
/// </summary>
/// <remarks>
/// <para>Attachment is the source tile searching for support, not the support pulling the source.</para>
/// </remarks>
public sealed class AttachmentSystem(ChunkStore store)
{
    private const int MaxCascadeDepth = 8;

    private bool _handling;
    private int _depth;
    private readonly HashSet<(int X, int Y, int Layer)> _processed = [];

    /// <summary>
    /// <para>Find support for a tile placement.</para>
    /// </summary>
    public AttachmentSearchResult Search(Chunk chunk, int layer, TilePlacementAnchor anchor, TileResource resource,
        TileVector2D playerCell)
    {
        if (resource.ProbeMask == 0)
        {
            return new AttachmentSearchResult(false, AttachmentDirections.None, new TileVector2D(), 0);
        }

        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            return new AttachmentSearchResult(false, AttachmentDirections.None, new TileVector2D(), 0);
        }

        var preferred = PreferredDirection(anchor.FocusCell, playerCell);
        foreach (var direction in SearchOrder(preferred))
        {
            if ((resource.ProbeMask & (int)direction) == 0)
            {
                continue;
            }

            if (direction == AttachmentDirections.Background)
            {
                for (var supportLayerIndex = layerIndex - 1; supportLayerIndex >= 0; supportLayerIndex--)
                {
                    if (CheckEdge(chunk, anchor, resource, direction, supportLayerIndex))
                    {
                        return new AttachmentSearchResult(true, direction, new TileVector2D(0, 0),
                            WorldConstants.IndexToLayer(supportLayerIndex));
                    }
                }
            }
            else if (CheckEdge(chunk, anchor, resource, direction, layerIndex))
            {
                return new AttachmentSearchResult(true, direction, DirectionOffset(direction), layer);
            }
        }

        return new AttachmentSearchResult(false, AttachmentDirections.None, new TileVector2D(), 0);
    }

    /// <summary>
    /// <para>React to a cell change by breaking attached tiles whose support disappeared.</para>
    /// </summary>
    public void OnCellChanged(Chunk chunk, int layer, int index, TileDestroySource destroySource)
    {
        if (destroySource is TileDestroySource.WorldGeneration or TileDestroySource.Load
            or TileDestroySource.Unload)
        {
            return;
        }

        if (_handling)
        {
            if (_depth >= MaxCascadeDepth)
            {
                return;
            }

            _depth++;
            try
            {
                Evaluate(chunk, layer, index);
            }
            finally
            {
                _depth--;
            }

            return;
        }

        _handling = true;
        _depth = 0;
        _processed.Clear();
        try
        {
            Evaluate(chunk, layer, index);
        }
        finally
        {
            _handling = false;
            _depth = 0;
            _processed.Clear();
        }
    }

    private void Evaluate(Chunk chunk, int layer, int index)
    {
        var currentCell = chunk.GetWorldCell(index);
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            return;
        }

        // Same-layer four neighbours. The candidate expects support in the direction of the changed cell.
        CheckNeighbour(currentCell + new TileVector2D(0, 1), layerIndex, AttachmentDirections.Down, currentCell);
        CheckNeighbour(currentCell + new TileVector2D(0, -1), layerIndex, AttachmentDirections.Up, currentCell);
        CheckNeighbour(currentCell + new TileVector2D(1, 0), layerIndex, AttachmentDirections.Left, currentCell);
        CheckNeighbour(currentCell + new TileVector2D(-1, 0), layerIndex, AttachmentDirections.Right, currentCell);

        // Front layers at the same cell expect background support in the changed layer.
        var forward = WorldConstants.ForwardLayer(layer);
        while (forward != 0)
        {
            var forwardIndex = WorldConstants.LayerToIndex(forward);
            CheckBackgroundNeighbour(currentCell, forwardIndex, layer, currentCell);
            forward = WorldConstants.ForwardLayer(forward);
        }
    }

    private void CheckNeighbour(TileVector2D candidateCell, int layerIndex, AttachmentDirections expected,
        TileVector2D changedCell)
    {
        var chunk = store.GetChunkAt(candidateCell);
        if (chunk is null)
        {
            return;
        }

        var index = chunk.IndexOf(candidateCell);
        var state = chunk.GetState(layerIndex, index);
        if (state is null || state.AttachmentDirection != expected)
        {
            return;
        }

        if (candidateCell.X + state.AttachmentOffset.X != changedCell.X ||
            candidateCell.Y + state.AttachmentOffset.Y != changedCell.Y)
        {
            return;
        }

        ValidateAndBreak(chunk, state, candidateCell, expected, state.AttachmentLayer);
    }

    private void CheckBackgroundNeighbour(TileVector2D candidateCell, int candidateLayerIndex, int changedLayer,
        TileVector2D changedCell)
    {
        var chunk = store.GetChunkAt(candidateCell);
        if (chunk is null)
        {
            return;
        }

        var index = chunk.IndexOf(candidateCell);
        var state = chunk.GetState(candidateLayerIndex, index);
        if (state is null || state.AttachmentDirection != AttachmentDirections.Background)
        {
            return;
        }

        if (state.AttachmentLayer != changedLayer)
        {
            return;
        }

        ValidateAndBreak(chunk, state, candidateCell, AttachmentDirections.Background, changedLayer);
    }

    private void ValidateAndBreak(Chunk chunk, TileState state, TileVector2D candidateCell,
        AttachmentDirections direction, int supportLayer)
    {
        var topLeft = state.GetTopLeft(candidateCell);
        var width = Math.Clamp(state.Width, 1, WorldConstants.ChunkSize);
        var height = Math.Clamp(state.Height, 1, WorldConstants.ChunkSize);

        // The tile's own layer is derived from the candidate chunk and cell. Find it from the cell state.
        var ownLayerIndex = FindLayerIndex(chunk, candidateCell, state);
        if (ownLayerIndex < 0)
        {
            return;
        }

        if (IsStillSupported(topLeft, width, height, direction, ownLayerIndex, supportLayer))
        {
            return;
        }

        BreakAttachedTile(chunk, topLeft, width, height, ownLayerIndex);
    }

    private bool IsStillSupported(TileVector2D topLeft, int width, int height, AttachmentDirections direction,
        int ownLayerIndex, int supportLayer)
    {
        if (direction == AttachmentDirections.Background)
        {
            var supportLayerIndex = WorldConstants.LayerToIndex(supportLayer);
            if (supportLayerIndex < 0)
            {
                return false;
            }

            for (var x = 0; x < width; x++)
            {
                for (var y = 0; y < height; y++)
                {
                    var cell = new TileVector2D(topLeft.X + x, topLeft.Y - y);
                    if (!HasSupport(cell, supportLayerIndex))
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        var offset = DirectionOffset(direction);
        foreach (var cell in EdgeCells(topLeft, width, height, direction))
        {
            if (!HasSupport(new TileVector2D(cell.X + offset.X, cell.Y + offset.Y), ownLayerIndex))
            {
                return false;
            }
        }

        return true;
    }

    private void BreakAttachedTile(Chunk chunk, TileVector2D topLeft, int width, int height, int layerIndex)
    {
        var key = (topLeft.X, topLeft.Y, layerIndex);
        if (!_processed.Add(key))
        {
            return;
        }

        for (var x = 0; x < width; x++)
        {
            for (var y = 0; y < height; y++)
            {
                var cell = new TileVector2D(topLeft.X + x, topLeft.Y - y);
                var cellChunk = store.GetChunkAt(cell);
                if (cellChunk is null)
                {
                    continue;
                }

                var cellIndex = cellChunk.IndexOf(cell);
                if (cellIndex >= 0 && cellChunk.GetState(layerIndex, cellIndex) is not null)
                {
                    cellChunk.ClearCell(layerIndex, cellIndex, TileDestroySource.SupportLost);
                }
            }
        }
    }

    private bool CheckEdge(Chunk chunk, TilePlacementAnchor anchor, TileResource resource,
        AttachmentDirections direction, int supportLayerIndex)
    {
        var supportLayer = WorldConstants.IndexToLayer(supportLayerIndex);
        var cells = direction == AttachmentDirections.Background
            ? AllCells(anchor)
            : EdgeCells(anchor.TopLeft, anchor.Width, anchor.Height, direction)
                .Select(cell =>
                {
                    var offset = DirectionOffset(direction);
                    return new TileVector2D(cell.X + offset.X, cell.Y + offset.Y);
                })
                .ToList();

        foreach (var cell in cells)
        {
            var supportChunk = store.GetChunkAt(cell);
            if (supportChunk is null)
            {
                return false;
            }

            var index = supportChunk.IndexOf(cell);
            var supportTile = supportChunk.GetTile(supportLayerIndex, index);
            if (supportTile is null)
            {
                return false;
            }

            var received = direction == AttachmentDirections.Background
                ? AttachmentDirections.Background
                : Opposite(direction);
            if ((supportTile.Resource.ReceiveMask & (int)received) == 0)
            {
                return false;
            }

            if (!TagFilterMatches(resource, supportTile.Resource))
            {
                return false;
            }
        }

        return true;
    }

    private static bool TagFilterMatches(TileResource source, TileResource support)
    {
        if (source.Tags.Count == 0)
        {
            return true;
        }

        foreach (var tag in source.Tags)
        {
            if (support.Tags.Contains(tag, StringComparer.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private bool HasSupport(TileVector2D cell, int layerIndex)
    {
        var chunk = store.GetChunkAt(cell);
        if (chunk is null)
        {
            return false;
        }

        var index = chunk.IndexOf(cell);
        return index >= 0 && chunk.GetTile(layerIndex, index) is not null;
    }

    private int FindLayerIndex(Chunk chunk, TileVector2D cell, TileState state)
    {
        var index = chunk.IndexOf(cell);
        if (index < 0)
        {
            return -1;
        }

        for (var layerIndex = 0; layerIndex < WorldConstants.LayerCount; layerIndex++)
        {
            if (ReferenceEquals(chunk.GetState(layerIndex, index), state))
            {
                return layerIndex;
            }
        }

        return -1;
    }

    private static AttachmentDirections PreferredDirection(TileVector2D focus, TileVector2D player)
    {
        var dx = focus.X - player.X;
        var dy = focus.Y - player.Y;
        if (dx == 0 && dy == 0)
        {
            return AttachmentDirections.Background;
        }

        if (dx != 0 && dy != 0)
        {
            return AttachmentDirections.Up;
        }

        if (dx > 0)
        {
            return AttachmentDirections.Right;
        }

        if (dx < 0)
        {
            return AttachmentDirections.Left;
        }

        return dy > 0 ? AttachmentDirections.Up : AttachmentDirections.Down;
    }

    private static IEnumerable<AttachmentDirections> SearchOrder(AttachmentDirections preferred)
    {
        if (preferred == AttachmentDirections.Background)
        {
            yield return AttachmentDirections.Background;
            yield return AttachmentDirections.Up;
            yield return AttachmentDirections.Right;
            yield return AttachmentDirections.Down;
            yield return AttachmentDirections.Left;
            yield break;
        }

        var order = preferred switch
        {
            AttachmentDirections.Up => new[]
            {
                AttachmentDirections.Up, AttachmentDirections.Right, AttachmentDirections.Down,
                AttachmentDirections.Left
            },
            AttachmentDirections.Right => new[]
            {
                AttachmentDirections.Right, AttachmentDirections.Down, AttachmentDirections.Left,
                AttachmentDirections.Up
            },
            AttachmentDirections.Down => new[]
            {
                AttachmentDirections.Down, AttachmentDirections.Left, AttachmentDirections.Up,
                AttachmentDirections.Right
            },
            _ => new[]
            {
                AttachmentDirections.Left, AttachmentDirections.Up, AttachmentDirections.Right,
                AttachmentDirections.Down
            }
        };

        foreach (var direction in order)
        {
            yield return direction;
        }

        yield return AttachmentDirections.Background;
    }

    private static IEnumerable<TileVector2D> AllCells(TilePlacementAnchor anchor)
    {
        for (var x = 0; x < anchor.Width; x++)
        {
            for (var y = 0; y < anchor.Height; y++)
            {
                yield return new TileVector2D(anchor.TopLeft.X + x, anchor.TopLeft.Y - y);
            }
        }
    }

    private static IEnumerable<TileVector2D> EdgeCells(TileVector2D topLeft, int width, int height,
        AttachmentDirections direction)
    {
        switch (direction)
        {
            case AttachmentDirections.Up:
                for (var x = 0; x < width; x++)
                {
                    yield return new TileVector2D(topLeft.X + x, topLeft.Y);
                }

                break;
            case AttachmentDirections.Down:
                for (var x = 0; x < width; x++)
                {
                    yield return new TileVector2D(topLeft.X + x, topLeft.Y - height + 1);
                }

                break;
            case AttachmentDirections.Right:
                for (var y = 0; y < height; y++)
                {
                    yield return new TileVector2D(topLeft.X + width - 1, topLeft.Y - y);
                }

                break;
            case AttachmentDirections.Left:
                for (var y = 0; y < height; y++)
                {
                    yield return new TileVector2D(topLeft.X, topLeft.Y - y);
                }

                break;
        }
    }

    internal static TileVector2D DirectionOffset(AttachmentDirections direction) => direction switch
    {
        AttachmentDirections.Up => new TileVector2D(0, 1),
        AttachmentDirections.Right => new TileVector2D(1, 0),
        AttachmentDirections.Down => new TileVector2D(0, -1),
        AttachmentDirections.Left => new TileVector2D(-1, 0),
        _ => new TileVector2D(0, 0)
    };

    internal static AttachmentDirections Opposite(AttachmentDirections direction) => direction switch
    {
        AttachmentDirections.Up => AttachmentDirections.Down,
        AttachmentDirections.Down => AttachmentDirections.Up,
        AttachmentDirections.Left => AttachmentDirections.Right,
        AttachmentDirections.Right => AttachmentDirections.Left,
        _ => AttachmentDirections.Background
    };
}

/// <summary>
/// <para>TilePlacementAnchor</para>
/// </summary>
public readonly record struct TilePlacementAnchor(
    TileVector2D FocusCell,
    TileVector2D TopLeft,
    int Width,
    int Height);
