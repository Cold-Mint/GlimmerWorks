using GlimmerWorks.Engine.math;

namespace GlimmerWorks.Engine.World;

/// <summary>
/// <para>WorldConstants</para>
/// </summary>
/// <remarks>
/// <para>Y axis points up. Chunk and terrain vertices are bottom-left corners.</para>
/// </remarks>
public static class WorldConstants
{
    /// <summary>
    /// <para>One tile is 16 world pixels.</para>
    /// </summary>
    public const int TilePixelSize = 16;

    /// <summary>
    /// <para>Chunk edge length in tiles.</para>
    /// </summary>
    public const int ChunkSize = 16;

    /// <summary>
    /// <para>Chunk size shift.</para>
    /// </summary>
    public const int ChunkShift = 4;

    /// <summary>
    /// <para>Chunk size mask.</para>
    /// </summary>
    public const int ChunkMask = ChunkSize - 1;

    /// <summary>
    /// <para>Cells per chunk.</para>
    /// </summary>
    public const int ChunkCellCount = ChunkSize * ChunkSize;

    /// <summary>
    /// <para>Chunk edge length in world pixels.</para>
    /// </summary>
    public const int ChunkPixelSize = ChunkSize * TilePixelSize;

    /// <summary>
    /// <para>Terrain edge length in tiles.</para>
    /// </summary>
    public const int TerrainSize = 256;

    /// <summary>
    /// <para>Terrain size shift.</para>
    /// </summary>
    public const int TerrainShift = 8;

    /// <summary>
    /// <para>Terrain size mask.</para>
    /// </summary>
    public const int TerrainMask = TerrainSize - 1;

    /// <summary>
    /// <para>Cells per terrain block.</para>
    /// </summary>
    public const int TerrainCellCount = TerrainSize * TerrainSize;

    /// <summary>
    /// <para>Chunks per terrain edge.</para>
    /// </summary>
    public const int TerrainChunkCount = TerrainSize / ChunkSize;

    /// <summary>
    /// <para>Number of tile layers.</para>
    /// </summary>
    public const int LayerCount = 3;

    /// <summary>
    /// <para>Background wall layer bit.</para>
    /// </summary>
    public const int BackgroundLayer = 1;

    /// <summary>
    /// <para>Background decoration layer bit.</para>
    /// </summary>
    public const int BackgroundDecorationLayer = 2;

    /// <summary>
    /// <para>Foreground layer bit.</para>
    /// </summary>
    public const int ForegroundLayer = 4;

    /// <summary>
    /// <para>Layer bits in draw order.</para>
    /// </summary>
    public static readonly int[] LayerBits = [BackgroundLayer, BackgroundDecorationLayer, ForegroundLayer];

    /// <summary>
    /// <para>Logical simulation rate.</para>
    /// </summary>
    public const int TicksPerSecond = 40;

    /// <summary>
    /// <para>Convert a tile coordinate to world pixels.</para>
    /// </summary>
    public static WorldVector2D TileToWorld(TileVector2D tile) =>
        new(tile.X * (float)TilePixelSize, tile.Y * (float)TilePixelSize);

    /// <summary>
    /// <para>Convert world pixels to the nearest tile. This rounds to nearest, not down.</para>
    /// </summary>
    public static TileVector2D WorldToTile(WorldVector2D world) => new(
        (int)MathF.Floor(world.X / TilePixelSize + 0.5F),
        (int)MathF.Floor(world.Y / TilePixelSize + 0.5F));

    /// <summary>
    /// <para>Bottom-left vertex of the chunk containing the tile.</para>
    /// </summary>
    public static ChunkVertexVector2D GetChunkVertex(TileVector2D tile) =>
        new(tile.X & ~ChunkMask, tile.Y & ~ChunkMask);

    /// <summary>
    /// <para>Chunk-local offset of the tile.</para>
    /// </summary>
    public static ChunkRelativeVector2D GetChunkRelative(TileVector2D tile) =>
        new((uint)(tile.X & ChunkMask), (uint)(tile.Y & ChunkMask));

    /// <summary>
    /// <para>Bottom-left vertex of the terrain block containing the tile.</para>
    /// </summary>
    public static TerrainVertexVector2D GetTerrainVertex(TileVector2D tile) =>
        new(tile.X & ~TerrainMask, tile.Y & ~TerrainMask);

    /// <summary>
    /// <para>Bottom-left vertex of the terrain block containing the chunk.</para>
    /// </summary>
    public static TerrainVertexVector2D GetTerrainVertex(ChunkVertexVector2D chunk) =>
        new(chunk.X & ~TerrainMask, chunk.Y & ~TerrainMask);

    /// <summary>
    /// <para>Terrain-local offset of the tile.</para>
    /// </summary>
    public static TerrainRelativeVector2D GetTerrainRelative(TileVector2D tile) =>
        new((uint)(tile.X & TerrainMask), (uint)(tile.Y & TerrainMask));

    /// <summary>
    /// <para>Rebuild a tile coordinate from a chunk vertex and local offset.</para>
    /// </summary>
    public static TileVector2D GetTile(ChunkVertexVector2D chunk, ChunkRelativeVector2D local) =>
        new(chunk.X + (int)local.X, chunk.Y + (int)local.Y);

    /// <summary>
    /// <para>Rebuild a tile coordinate from a terrain vertex and local offset.</para>
    /// </summary>
    public static TileVector2D GetTile(TerrainVertexVector2D terrain, TerrainRelativeVector2D local) =>
        new(terrain.X + (int)local.X, terrain.Y + (int)local.Y);

    /// <summary>
    /// <para>Row-major chunk index. X must be in [0,15].</para>
    /// </summary>
    public static int ChunkIndex(int x, int y) => (y << ChunkShift) | x;

    /// <summary>
    /// <para>Row-major chunk index from a chunk-local vector.</para>
    /// </summary>
    public static int ChunkIndex(ChunkRelativeVector2D local) => (int)((local.Y << ChunkShift) | local.X);

    /// <summary>
    /// <para>Row-major terrain index. X must be in [0,255].</para>
    /// </summary>
    public static int TerrainIndex(int x, int y) => (y << TerrainShift) | x;

    /// <summary>
    /// <para>Row-major terrain index from a terrain-local vector.</para>
    /// </summary>
    public static int TerrainIndex(TerrainRelativeVector2D local) =>
        (int)((local.Y << TerrainShift) | local.X);

    /// <summary>
    /// <para>X component of a chunk index.</para>
    /// </summary>
    public static int ChunkIndexX(int index) => index & ChunkMask;

    /// <summary>
    /// <para>Y component of a chunk index.</para>
    /// </summary>
    public static int ChunkIndexY(int index) => index >> ChunkShift;

    /// <summary>
    /// <para>X component of a terrain index.</para>
    /// </summary>
    public static int TerrainIndexX(int index) => index & TerrainMask;

    /// <summary>
    /// <para>Y component of a terrain index.</para>
    /// </summary>
    public static int TerrainIndexY(int index) => index >> TerrainShift;

    /// <summary>
    /// <para>Map a layer bit (1/2/4) to its array index (0/1/2). Returns -1 for invalid values.</para>
    /// </summary>
    public static int LayerToIndex(int layer) => layer switch
    {
        1 => 0,
        2 => 1,
        4 => 2,
        _ => -1
    };

    /// <summary>
    /// <para>Map an array index (0/1/2) to its layer bit (1/2/4).</para>
    /// </summary>
    public static int IndexToLayer(int index) => index switch
    {
        0 => 1,
        1 => 2,
        2 => 4,
        _ => 0
    };

    /// <summary>
    /// <para>Layer in front of the given one, or 0 when there is none.</para>
    /// </summary>
    public static int ForwardLayer(int layer) => layer switch
    {
        1 => 2,
        2 => 4,
        _ => 0
    };

    /// <summary>
    /// <para>Layer behind the given one, or 0 when there is none.</para>
    /// </summary>
    public static int BackwardLayer(int layer) => layer switch
    {
        2 => 1,
        4 => 2,
        _ => 0
    };

    /// <summary>
    /// <para>Whether a tile coordinate lies inside the dimension bounds.</para>
    /// </summary>
    public static bool InDimension(Mods.Resources.DimensionResource dimension, int x, int y) =>
        x >= dimension.MinX && x <= dimension.MaxX && y >= dimension.MinY && y <= dimension.MaxY;
}
