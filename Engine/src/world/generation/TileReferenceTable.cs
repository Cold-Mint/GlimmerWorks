using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.World.Tiles;

namespace GlimmerWorks.Engine.World.Generation;

/// <summary>
/// <para>TileReferenceTable</para>
/// </summary>
public sealed class TileReferenceTable
{
    private readonly ResourceRef?[][] _layers =
    [
        new ResourceRef?[WorldConstants.ChunkCellCount],
        new ResourceRef?[WorldConstants.ChunkCellCount],
        new ResourceRef?[WorldConstants.ChunkCellCount]
    ];

    /// <summary>
    /// <para>Get a reference by layer bit. An invalid layer throws.</para>
    /// </summary>
    public ResourceRef? Get(int layer, int index)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(layer), layer, "Layer must be 1, 2 or 4.");
        }

        if (index is < 0 or >= WorldConstants.ChunkCellCount)
        {
            throw new ArgumentOutOfRangeException(nameof(index), index, "Chunk index out of range.");
        }

        return _layers[layerIndex][index];
    }

    /// <summary>
    /// <para>Set a reference by layer bit.</para>
    /// </summary>
    public void Set(int layer, int index, ResourceRef? reference)
    {
        var layerIndex = WorldConstants.LayerToIndex(layer);
        if (layerIndex < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(layer), layer, "Layer must be 1, 2 or 4.");
        }

        if (index is < 0 or >= WorldConstants.ChunkCellCount)
        {
            throw new ArgumentOutOfRangeException(nameof(index), index, "Chunk index out of range.");
        }

        _layers[layerIndex][index] = reference;
    }

    /// <summary>
    /// <para>Get a reference by layer array index (0/1/2).</para>
    /// </summary>
    public ResourceRef? GetByIndex(int layerIndex, int index) =>
        layerIndex is >= 0 and < WorldConstants.LayerCount ? _layers[layerIndex][index] : null;

    /// <summary>
    /// <para>Set a reference by layer array index (0/1/2).</para>
    /// </summary>
    public void SetByIndex(int layerIndex, int index, ResourceRef? reference)
    {
        if (layerIndex is < 0 or >= WorldConstants.LayerCount || index is < 0 or >= WorldConstants.ChunkCellCount)
        {
            return;
        }

        _layers[layerIndex][index] = reference;
    }

    /// <summary>
    /// <para>Copy the table into a flat snapshot. Used by tests and diagnostics.</para>
    /// </summary>
    public ResourceRef?[] Flatten(int layer) => (ResourceRef?[])_layers[WorldConstants.LayerToIndex(layer)].Clone();
}
