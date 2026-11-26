namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of terrain-relative coordinates.
/// 地形相对坐标二维向量。
/// Unit: Tile. 单位：瓦片。
/// </summary>
public sealed class TerrainRelativeVector2D : BaseVector<uint, TerrainRelativeVector2D>
{
    public TerrainRelativeVector2D()
    {
    }

    public TerrainRelativeVector2D(uint x, uint y) : base(x, y)
    {
    }
}
