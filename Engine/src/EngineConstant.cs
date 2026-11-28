namespace GlimmerWorks.Engine;

public static class EngineConstant
{
    /// <summary>
    /// <para>GameCode</para>
    /// <para>游戏版本代号</para>
    /// </summary>
    public const long GameCode = 4;

    /// <summary>
    /// <para>CacheCode</para>
    /// <para>缓存系统的版本</para>
    /// </summary>
    /// <remarks>
    ///<para>Modifying the version number will cause the old cache to become invalid.</para>
    ///<para>修改版本号会让旧缓存失效。</para>
    /// </remarks>
    public const long CacheCode = 1;

    /// <summary>
    /// <para>How many Ticks are executed per second?</para>
    /// <para>每秒执行多少Tick</para>
    /// </summary>
    public const uint TicksPerSecond = 25;

    /// <summary>
    /// <para>Tiles along one edge of a chunk.</para>
    /// <para>一个区块的边长（瓦片数）。</para>
    /// </summary>
    public const int ChunkEdgeTiles = 16;

    /// <summary>
    /// <para>Total tiles in one chunk.</para>
    /// <para>一个区块内的瓦片总数。</para>
    /// </summary>
    public const int ChunkTileCount = ChunkEdgeTiles * ChunkEdgeTiles;

    /// <summary>
    /// <para>Chunks along one edge of a terrain set.</para>
    /// <para>地形集合的边长（区块数）。</para>
    /// </summary>
    public const int TerrainSetEdgeChunks = 16;


    /// <summary>
    /// <para>Tiles along one edge of a terrain set.</para>
    /// <para>地形集合的边长（瓦片数）。</para>
    /// </summary>
    public const int TerrainSetEdgeTiles = TerrainSetEdgeChunks * ChunkEdgeTiles;

    /// <summary>
    /// <para>Total tiles in one terrain set.</para>
    /// <para>一个地形集合内的瓦片总数。</para>
    /// </summary>
    public const int TerrainSetTileCount = TerrainSetEdgeTiles * TerrainSetEdgeTiles;
}