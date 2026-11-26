namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of terrain vertex coordinates.
/// 地形顶点坐标二维向量。
/// Unit: Tile. 单位：瓦片。
/// </summary>
public sealed class TerrainVertexVector2D : BaseVector<int, TerrainVertexVector2D>
{
    public TerrainVertexVector2D()
    {
    }

    public TerrainVertexVector2D(int x, int y) : base(x, y)
    {
    }
}
