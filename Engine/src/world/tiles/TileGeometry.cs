using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Tiles;

/// <summary>
/// <para>TileGeometry</para>
/// <para>大型瓦片的几何计算。原点格是左下角，满足 offset = (0, H-1)。</para>
/// </summary>
public static class TileGeometry
{
    /// <summary>
    /// <para>Whether the state belongs to the bottom-left origin cell.</para>
    /// </summary>
    public static bool IsOrigin(TileState state) => state.Offset.X == 0 && state.Offset.Y == state.Height - 1;

    /// <summary>
    /// <para>Top-left cell from any covered cell and its state.</para>
    /// </summary>
    public static TileVector2D GetTopLeft(TileVector2D cell, TileState state) =>
        new(cell.X + state.Offset.X, cell.Y + state.Offset.Y);

    /// <summary>
    /// <para>Bottom-left origin cell from a top-left cell and the tile height.</para>
    /// </summary>
    public static TileVector2D GetOriginCell(TileVector2D topLeft, int height) =>
        new(topLeft.X, topLeft.Y - Math.Clamp(height, 1, WorldConstants.ChunkSize) + 1);

    /// <summary>
    /// <para>Enumerate every covered cell from the top-left cell, top row first.</para>
    /// </summary>
    public static IEnumerable<TileVector2D> EnumerateCells(TileVector2D topLeft, int width, int height)
    {
        width = Math.Clamp(width, 1, WorldConstants.ChunkSize);
        height = Math.Clamp(height, 1, WorldConstants.ChunkSize);
        for (var x = 0; x < width; x++)
        {
            for (var y = 0; y < height; y++)
            {
                yield return new TileVector2D(topLeft.X + x, topLeft.Y - y);
            }
        }
    }

    /// <summary>
    /// <para>Centre cell used by mining and loot settlement.</para>
    /// </summary>
    public static TileVector2D GetCenter(TileVector2D topLeft, int width, int height) =>
        new(topLeft.X + Math.Clamp(width, 1, 16) / 2, topLeft.Y - Math.Clamp(height, 1, 16) / 2);

    /// <summary>
    /// <para>Total hardness. By default it scales with the covered area.</para>
    /// </summary>
    public static float GetHardness(TileResource resource, int width, int height)
    {
        var unit = Math.Max(0.0F, resource.UnitHardness);
        return resource.AutoHardnessScale ? unit * Math.Max(1, width) * Math.Max(1, height) : unit;
    }

    /// <summary>
    /// <para>Durability/unit-dig cost should be charged at each covered cell when auto-scaling.</para>
    /// </summary>
    public static int GetDigCost(TileResource resource, int width, int height)
    {
        var unit = Math.Max(1, resource.UnitDigCost);
        return resource.AutoDigCostScale
            ? unit * Math.Max(1, width) * Math.Max(1, height)
            : unit;
    }

    /// <summary>
    /// <para>Whether this covered cell should settle loot. By default only the centre cell drops.</para>
    /// </summary>
    public static bool IsLootCell(TileResource resource, TileState state, TileVector2D coveredCell)
    {
        if (!resource.LootOnlyAtCenter)
        {
            return true;
        }

        var topLeft = GetTopLeft(coveredCell, state);
        return coveredCell.Equals(GetCenter(topLeft, state.Width, state.Height));
    }
}
