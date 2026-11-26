namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of chunk vertex coordinates.
/// 区块顶点坐标二维向量。
/// Unit: Tile. 单位：瓦片。
/// </summary>
public sealed class ChunkVertexVector2D : BaseVector<int, ChunkVertexVector2D>
{
    public ChunkVertexVector2D()
    {
    }

    public ChunkVertexVector2D(int x, int y) : base(x, y)
    {
    }
}
