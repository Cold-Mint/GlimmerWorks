namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of world coordinates.
/// 世界坐标二维向量。
/// The unit of the world coordinate is logical pixels. It is affected by the camera's zoom.
/// 世界坐标的单位是逻辑像素。会受到相机缩放的影响。
/// For example: X + 1, when scaled by a factor of 2, then it actually moves 2 screen pixels to the right.
/// 例如：X+1，缩放为2倍，那么实际向右偏移了2屏幕像素。
/// </summary>
public sealed class WorldVector2D : BaseVector<float, WorldVector2D>
{
    public WorldVector2D()
    {
    }

    public WorldVector2D(float x, float y) : base(x, y)
    {
    }
}
