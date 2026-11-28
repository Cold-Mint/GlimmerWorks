using GlimmerWorks.Engine.Math;

namespace GlimmerWorks.Engine.World.Terrain;

/// <summary>
/// <para>Terrain</para>
/// <para>地形</para>
/// </summary>
public class TerrainSet
{
    private readonly TerrainTile[] _terrainTiles =
        new TerrainTile[EngineConstant.TerrainSetTileCount];

    /// <summary>
    /// <para>Read-only view of all terrain tiles, indexed by <c>y * TerrainSetEdgeTiles + x</c>.</para>
    /// <para>所有地形格的只读视图，索引为 <c>y * TerrainSetEdgeTiles + x</c>。</para>
    /// </summary>
    public ReadOnlySpan<TerrainTile> TerrainTiles => _terrainTiles;

    public void SetTerrainTile(uint index, TerrainTile terrainTile)
    {
        _terrainTiles[index] = terrainTile;
    }

    public void SetTerrainTile(TerrainRelativeVector terrainRelativeVector, TerrainTile terrainTile)
    {
        _terrainTiles[terrainRelativeVector.ToTerrainTileIndex()] = terrainTile;
    }

    public void SetTerrainTile(TileVector tileVector, TerrainTile terrainTile)
    {
        _terrainTiles[tileVector.ToTerrainTileIndex()] = terrainTile;
    }

    public TerrainTile GetTerrainTile(TerrainRelativeVector terrainRelativeVector)
    {
        return _terrainTiles[terrainRelativeVector.ToTerrainTileIndex()];
    }

    public TerrainTile GetTerrainTile(TileVector tileVector)
    {
        return _terrainTiles[tileVector.ToTerrainTileIndex()];
    }

    public TerrainTile GetTerrainTile(int index)
    {
        return _terrainTiles[index];
    }
}