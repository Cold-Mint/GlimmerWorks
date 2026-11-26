namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of tile coordinates.
/// 瓦片二维向量。
/// Unit: Tile. 单位：瓦片。
/// </summary>
public sealed class TileVector2D : BaseVector<int, TileVector2D>
{
    public TileVector2D()
    {
    }

    public TileVector2D(int x, int y) : base(x, y)
    {
    }
}
