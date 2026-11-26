using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Placement;

/// <summary>
/// <para>TileAnchorKind</para>
/// </summary>
public enum TileAnchorKind
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Custom
}

/// <summary>
/// <para>TilePlacementPlan</para>
/// </summary>
public readonly record struct TilePlacementPlan(
    TileVector2D BottomLeft,
    TileVector2D TopLeft,
    int AnchorX,
    int AnchorY,
    int Width,
    int Height);

/// <summary>
/// <para>TilePlacementCalculator</para>
/// </summary>
public static class TilePlacementCalculator
{
    /// <summary>
    /// <para>Parse an anchor name. Unknown names fall back to bottom_left.</para>
    /// </summary>
    public static TileAnchorKind ParseAnchor(string value) => value.ToLowerInvariant() switch
    {
        "top_left" => TileAnchorKind.TopLeft,
        "top_center" => TileAnchorKind.TopCenter,
        "top_right" => TileAnchorKind.TopRight,
        "center_left" => TileAnchorKind.CenterLeft,
        "center" => TileAnchorKind.Center,
        "center_right" => TileAnchorKind.CenterRight,
        "bottom_center" => TileAnchorKind.BottomCenter,
        "bottom_right" => TileAnchorKind.BottomRight,
        "custom" => TileAnchorKind.Custom,
        _ => TileAnchorKind.BottomLeft
    };

    /// <summary>
    /// <para>Anchor relative to the bottom-left corner, clamped to the tile bounds.</para>
    /// </summary>
    public static (int X, int Y) GetAnchor(TileResource resource)
    {
        var width = resource.ClampedWidth;
        var height = resource.ClampedHeight;
        var kind = ParseAnchor(resource.Anchor);
        var (x, y) = kind switch
        {
            TileAnchorKind.TopLeft => (0, height - 1),
            TileAnchorKind.TopCenter => (width / 2, height - 1),
            TileAnchorKind.TopRight => (width - 1, height - 1),
            TileAnchorKind.CenterLeft => (0, height / 2),
            TileAnchorKind.Center => (width / 2, height / 2),
            TileAnchorKind.CenterRight => (width - 1, height / 2),
            TileAnchorKind.BottomCenter => (width / 2, 0),
            TileAnchorKind.BottomRight => (width - 1, 0),
            TileAnchorKind.Custom => (resource.AnchorX, resource.AnchorY),
            _ => (0, 0)
        };

        return (Math.Clamp(x, 0, width - 1), Math.Clamp(y, 0, height - 1));
    }

    /// <summary>
    /// <para>Compute the bottom-left and top-left cells for a player placement.</para>
    /// </summary>
    public static TilePlacementPlan Compute(TileResource resource, TileVector2D focusCell, TileVector2D playerCell,
        bool allowFacingAdjust)
    {
        var width = resource.ClampedWidth;
        var height = resource.ClampedHeight;
        var (anchorX, anchorY) = GetAnchor(resource);

        if (allowFacingAdjust && focusCell.X < playerCell.X)
        {
            anchorX = width - 1 - anchorX;
        }

        var bottomLeft = new TileVector2D(focusCell.X - anchorX, focusCell.Y - anchorY);
        var topLeft = new TileVector2D(bottomLeft.X, bottomLeft.Y + height - 1);
        return new TilePlacementPlan(bottomLeft, topLeft, anchorX, anchorY, width, height);
    }

    /// <summary>
    /// <para>Top-left cell minus current cell.</para>
    /// </summary>
    public static TileVector2D ComputeOffset(TileVector2D topLeft, TileVector2D current) =>
        new(topLeft.X - current.X, topLeft.Y - current.Y);
}
