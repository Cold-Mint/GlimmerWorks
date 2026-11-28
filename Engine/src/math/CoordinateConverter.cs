namespace GlimmerWorks.Engine.Math;

/// <summary>
/// <para>Conversions between tile, chunk and terrain-set coordinates.</para>
/// <para>瓦片、区块和地形集合坐标之间的转换。</para>
/// </summary>
/// <remarks>
/// <para>
/// Tile and vertex vectors use world tile coordinates. Relative vectors are local
/// offsets in the range [0, ChunkEdgeTiles) or [0, TerrainSetEdgeTiles).
/// </para>
/// <para>
/// TileVector 和 VertexVector 使用世界瓦片坐标；RelativeVector 是局部偏移，
/// 范围分别是 [0, ChunkEdgeTiles) 和 [0, TerrainSetEdgeTiles)。
/// </para>
/// </remarks>
public static class CoordinateConverter
{
    private const int ChunkMask = EngineConstant.ChunkEdgeTiles - 1;
    private const int TerrainMask = EngineConstant.TerrainSetEdgeTiles - 1;

    // ---------------------------------------------------------------------
    // TileVector -> other coordinates
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Get the bottom-left vertex of the chunk containing the tile.</para>
    /// <para>获取包含该瓦片的区块左下角顶点。</para>
    /// </summary>
    public static ChunkVertexVector ToChunkVertex(this TileVector tile) =>
        new(FloorToChunk(tile.X), FloorToChunk(tile.Y));

    /// <summary>
    /// <para>Get the tile offset inside its chunk.</para>
    /// <para>获取该瓦片在其区块内的局部坐标。</para>
    /// </summary>
    public static ChunkRelativeVector ToChunkRelative(this TileVector tile) =>
        new(ChunkLocal(tile.X), ChunkLocal(tile.Y));

    /// <summary>
    /// <para>Get the bottom-left vertex of the terrain set containing the tile.</para>
    /// <para>获取包含该瓦片的地形集合左下角顶点。</para>
    /// </summary>
    public static TerrainVertexVector ToTerrainVertex(this TileVector tile) =>
        new(FloorToTerrainSet(tile.X), FloorToTerrainSet(tile.Y));

    /// <summary>
    /// <para>Get the tile offset inside its terrain set.</para>
    /// <para>获取该瓦片在其地形集合内的局部坐标。</para>
    /// </summary>
    public static TerrainRelativeVector ToTerrainRelative(this TileVector tile) =>
        new(TerrainLocal(tile.X), TerrainLocal(tile.Y));

    // ---------------------------------------------------------------------
    // ChunkVertexVector -> other coordinates
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Get the bottom-left vertex of the terrain set containing the chunk.</para>
    /// <para>获取包含该区块的地形集合左下角顶点。</para>
    /// </summary>
    public static TerrainVertexVector ToTerrainVertex(this ChunkVertexVector chunk) =>
        new(FloorToTerrainSet(chunk.X), FloorToTerrainSet(chunk.Y));

    /// <summary>
    /// <para>Get the chunk's tile offset inside its terrain set.</para>
    /// <para>获取该区块在其地形集合内的瓦片偏移。</para>
    /// </summary>
    public static TerrainRelativeVector ToTerrainRelative(this ChunkVertexVector chunk) =>
        new(TerrainLocal(chunk.X), TerrainLocal(chunk.Y));

    // ---------------------------------------------------------------------
    // Local -> local
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Convert a terrain-local tile offset to its chunk-local tile offset.</para>
    /// <para>把地形集合内的局部坐标转换为所在区块内的局部坐标。</para>
    /// </summary>
    public static ChunkRelativeVector ToChunkRelative(this TerrainRelativeVector local) =>
        new(local.X & ChunkMask, local.Y & ChunkMask);

    // ---------------------------------------------------------------------
    // Vertex + local -> TileVector
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Rebuild a world tile from a chunk vertex and a chunk-local offset.</para>
    /// <para>由区块左下角和区块内局部坐标还原世界瓦片坐标。</para>
    /// </summary>
    public static TileVector ToTile(this ChunkVertexVector chunk, ChunkRelativeVector local) =>
        new(chunk.X + (int)local.X, chunk.Y + (int)local.Y);

    /// <summary>
    /// <para>Rebuild a world tile from a terrain-set vertex and a terrain-local offset.</para>
    /// <para>由地形集合左下角和地形集合内局部坐标还原世界瓦片坐标。</para>
    /// </summary>
    public static TileVector ToTile(this TerrainVertexVector terrain, TerrainRelativeVector local) =>
        new(terrain.X + (int)local.X, terrain.Y + (int)local.Y);

    // ---------------------------------------------------------------------
    // Combined conversions
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Convert a chunk-local offset to the terrain-local offset of the same tile.</para>
    /// <para>把区块内局部坐标转换为同一瓦片在地形集合内的局部坐标。</para>
    /// </summary>
    public static TerrainRelativeVector ToTerrainRelative(
        this ChunkVertexVector chunk,
        ChunkRelativeVector local)
    {
        var offset = ToTerrainRelative(chunk);
        return new TerrainRelativeVector(offset.X + local.X, offset.Y + local.Y);
    }

    /// <summary>
    /// <para>Get the bottom-left vertex of the chunk containing a terrain-local tile.</para>
    /// <para>获取包含某个地形集合内瓦片的区块左下角顶点。</para>
    /// </summary>
    public static ChunkVertexVector ToChunkVertex(
        this TerrainVertexVector terrain,
        TerrainRelativeVector local) =>
        new(
            terrain.X + ((int)local.X & ~ChunkMask),
            terrain.Y + ((int)local.Y & ~ChunkMask));

    // ---------------------------------------------------------------------
    // Index helpers
    // ---------------------------------------------------------------------

    /// <summary>
    /// <para>Flat index of a chunk-local tile: <c>y * ChunkEdgeTiles + x</c>.</para>
    /// <para>区块内瓦片的扁平索引：<c>y * ChunkEdgeTiles + x</c>。</para>
    /// </summary>
    public static int ToChunkTileIndex(this ChunkRelativeVector local) =>
        (int)(local.Y * EngineConstant.ChunkEdgeTiles + local.X);

    /// <summary>
    /// <para>Flat index of a tile inside its chunk.</para>
    /// <para>瓦片在其区块内的扁平索引。</para>
    /// </summary>
    public static int ToChunkTileIndex(this TileVector tile) =>
        ToChunkTileIndex(ToChunkRelative(tile));

    /// <summary>
    /// <para>Flat index of a terrain-local tile: <c>y * TerrainSetEdgeTiles + x</c>.</para>
    /// <para>地形集合内瓦片的扁平索引：<c>y * TerrainSetEdgeTiles + x</c>。</para>
    /// </summary>
    public static int ToTerrainTileIndex(this TerrainRelativeVector local) =>
        (int)(local.Y * EngineConstant.TerrainSetEdgeTiles + local.X);

    /// <summary>
    /// <para>Flat index of a tile inside its terrain set.</para>
    /// <para>瓦片在其地形集合内的扁平索引。</para>
    /// </summary>
    public static int ToTerrainTileIndex(this TileVector tile) =>
        ToTerrainTileIndex(ToTerrainRelative(tile));

    /// <summary>
    /// <para>Index of the chunk inside its terrain set: <c>chunkY * TerrainSetEdgeChunks + chunkX</c>.</para>
    /// <para>区块在其地形集合内的槽位索引：<c>chunkY * TerrainSetEdgeChunks + chunkX</c>。</para>
    /// </summary>
    public static int ToTerrainChunkIndex(this ChunkVertexVector chunk)
    {
        var x = (chunk.X & TerrainMask) / EngineConstant.ChunkEdgeTiles;
        var y = (chunk.Y & TerrainMask) / EngineConstant.ChunkEdgeTiles;
        return y * EngineConstant.TerrainSetEdgeChunks + x;
    }

    /// <summary>
    /// <para>Index of the chunk containing a world tile inside its terrain set.</para>
    /// <para>包含某个世界瓦片的区块在其地形集合内的槽位索引。</para>
    /// </summary>
    public static int ToTerrainChunkIndex(this TileVector tile) =>
        ToTerrainChunkIndex(ToChunkVertex(tile));

    /// <summary>
    /// <para>Index of the chunk containing a terrain-local tile.</para>
    /// <para>包含某个地形集合内瓦片的区块槽位索引。</para>
    /// </summary>
    public static int ToTerrainChunkIndex(this TerrainRelativeVector local)
    {
        var x = (int)(local.X / EngineConstant.ChunkEdgeTiles);
        var y = (int)(local.Y / EngineConstant.ChunkEdgeTiles);
        return y * EngineConstant.TerrainSetEdgeChunks + x;
    }

    /// <summary>
    /// <para>Rebuild a chunk-local offset from its flat index.</para>
    /// <para>由扁平索引还原区块内局部坐标。</para>
    /// </summary>
    public static ChunkRelativeVector FromChunkTileIndex(int index)
    {
        ArgumentOutOfRangeException.ThrowIfNegative(index);
        ArgumentOutOfRangeException.ThrowIfGreaterThanOrEqual(index, EngineConstant.ChunkTileCount);
        return new ChunkRelativeVector(
            (uint)(index % EngineConstant.ChunkEdgeTiles),
            (uint)(index / EngineConstant.ChunkEdgeTiles));
    }

    /// <summary>
    /// <para>Rebuild a terrain-local tile offset from its flat index.</para>
    /// <para>由扁平索引还原地形集合内的局部坐标。</para>
    /// </summary>
    public static TerrainRelativeVector FromTerrainTileIndex(int index)
    {
        ArgumentOutOfRangeException.ThrowIfNegative(index);
        ArgumentOutOfRangeException.ThrowIfGreaterThanOrEqual(index, EngineConstant.TerrainSetTileCount);
        return new TerrainRelativeVector(
            (uint)(index % EngineConstant.TerrainSetEdgeTiles),
            (uint)(index / EngineConstant.TerrainSetEdgeTiles));
    }

    /// <summary>
    /// <para>Rebuild a world tile from a chunk vertex and a chunk-local flat index.</para>
    /// <para>由区块左下角和区块内扁平索引还原世界瓦片坐标。</para>
    /// </summary>
    public static TileVector ToTile(this ChunkVertexVector chunk, int chunkTileIndex) =>
        chunk.ToTile(FromChunkTileIndex(chunkTileIndex));

    /// <summary>
    /// <para>Rebuild a world tile from a terrain-set vertex and a terrain-local flat index.</para>
    /// <para>由地形集合左下角和地形集合内扁平索引还原世界瓦片坐标。</para>
    /// </summary>
    public static TileVector ToTile(this TerrainVertexVector terrain, int terrainTileIndex) =>
        terrain.ToTile(FromTerrainTileIndex(terrainTileIndex));

    // ---------------------------------------------------------------------
    // Bit-mask helpers
    // ---------------------------------------------------------------------

    // All shifts rely on ChunkEdgeTiles and TerrainSetEdgeTiles being powers of two.
    // 所有位运算都依赖 ChunkEdgeTiles 和 TerrainSetEdgeTiles 是 2 的幂。
    private static int FloorToChunk(int value) => value & ~ChunkMask;

    private static int FloorToTerrainSet(int value) => value & ~TerrainMask;

    private static uint ChunkLocal(int value) => (uint)(value & ChunkMask);

    private static uint TerrainLocal(int value) => (uint)(value & TerrainMask);
}
