using GlimmerWorks.Engine.math;
using GlimmerWorks.Engine.Mods;
using GlimmerWorks.Engine.Mods.Resources;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>TerrainType</para>
/// </summary>
[Flags]
public enum TerrainType
{
    Empty = 1,
    Air = 2,
    Water = 4,
    Solid = 8,
    Structure = 16,
    Bedrock = 32
}

/// <summary>
/// <para>TerrainCell</para>
/// </summary>
public struct TerrainCell
{
    /// <summary>
    /// <para>World tile coordinate.</para>
    /// </summary>
    public TileVector2D Position;

    /// <summary>
    /// <para>Single terrain type.</para>
    /// </summary>
    public TerrainType Type;

    /// <summary>
    /// <para>Best matching biome. May be null.</para>
    /// </summary>
    public BiomeResource? Biome;

    /// <summary>
    /// <para>Structure tile for the background wall layer.</para>
    /// </summary>
    public ResourceRef? BackgroundWallTile;

    /// <summary>
    /// <para>Structure tile for the background decoration layer.</para>
    /// </summary>
    public ResourceRef? BackgroundDecorationTile;

    /// <summary>
    /// <para>Structure tile for the foreground layer.</para>
    /// </summary>
    public ResourceRef? ForegroundTile;

    public ResourceRef? GetStructureTile(int layerIndex) => layerIndex switch
    {
        0 => BackgroundWallTile,
        1 => BackgroundDecorationTile,
        2 => ForegroundTile,
        _ => null
    };

    public void SetStructureTile(int layerIndex, ResourceRef? tile)
    {
        switch (layerIndex)
        {
            case 0:
                BackgroundWallTile = tile;
                break;
            case 1:
                BackgroundDecorationTile = tile;
                break;
            case 2:
                ForegroundTile = tile;
                break;
        }
    }

    /// <summary>
    /// <para>Whether any structure tile is present.</para>
    /// </summary>
    public bool HasStructureTiles =>
        BackgroundWallTile is not null || BackgroundDecorationTile is not null || ForegroundTile is not null;
}

/// <summary>
/// <para>TerrainBlock</para>
/// </summary>
public sealed class TerrainBlock
{
    public TerrainBlock(TerrainVertexVector2D position)
    {
        Position = position;
        Cells = new TerrainCell[WorldConstants.TerrainCellCount];
    }

    /// <summary>
    /// <para>Bottom-left terrain vertex.</para>
    /// </summary>
    public TerrainVertexVector2D Position { get; }

    /// <summary>
    /// <para>Cell results, indexed by <c>y * 256 + x</c>.</para>
    /// </summary>
    public TerrainCell[] Cells { get; }

    /// <summary>
    /// <para>Structure placements staged before commit. Cleared after each structure.</para>
    /// </summary>
    public List<StructurePlacement> StructureStaging { get; } = [];

    /// <summary>
    /// <para>Get a cell by terrain-local coordinates.</para>
    /// </summary>
    public TerrainCell GetCell(int localX, int localY) =>
        Cells[WorldConstants.TerrainIndex(localX, localY)];

    /// <summary>
    /// <para>Get a cell by world tile coordinate.</para>
    /// </summary>
    public TerrainCell? GetCell(TileVector2D worldCell)
    {
        var localX = worldCell.X - Position.X;
        var localY = worldCell.Y - Position.Y;
        if (localX is < 0 or >= WorldConstants.TerrainSize ||
            localY is < 0 or >= WorldConstants.TerrainSize)
        {
            return null;
        }

        return Cells[WorldConstants.TerrainIndex(localX, localY)];
    }

    /// <summary>
    /// <para>Try to get a cell by world tile coordinate.</para>
    /// </summary>
    public bool TryGetCell(TileVector2D worldCell, out TerrainCell cell)
    {
        var found = GetCell(worldCell);
        cell = found ?? default;
        return found.HasValue;
    }

    /// <summary>
    /// <para>Set a cell by terrain-local coordinates.</para>
    /// </summary>
    public void SetCell(int localX, int localY, TerrainCell cell) =>
        Cells[WorldConstants.TerrainIndex(localX, localY)] = cell;

    /// <summary>
    /// <para>Whether a world cell lies inside this terrain block.</para>
    /// </summary>
    public bool Contains(TileVector2D worldCell) =>
        worldCell.X >= Position.X && worldCell.X < Position.X + WorldConstants.TerrainSize &&
        worldCell.Y >= Position.Y && worldCell.Y < Position.Y + WorldConstants.TerrainSize;

    /// <summary>
    /// <para>Terrain-local index of a world cell, or -1 when outside.</para>
    /// </summary>
    public int IndexOf(TileVector2D worldCell)
    {
        if (!Contains(worldCell))
        {
            return -1;
        }

        return WorldConstants.TerrainIndex(worldCell.X - Position.X, worldCell.Y - Position.Y);
    }
}

/// <summary>
/// <para>ClimateSample</para>
/// </summary>
public readonly record struct ClimateSample(
    float Humidity,
    float Temperature,
    float Weirdness,
    float Erosion,
    float Elevation,
    float SurfaceProximity,
    int SurfaceHeight);
