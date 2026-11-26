namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of chunk-relative coordinates.
/// 区块相对坐标二维向量。
/// Unit: Tile. 单位：瓦片。
/// </summary>
public sealed class ChunkRelativeVector2D : BaseVector<uint, ChunkRelativeVector2D>
{
    public ChunkRelativeVector2D()
    {
    }

    public ChunkRelativeVector2D(uint x, uint y) : base(x, y)
    {
    }
}
