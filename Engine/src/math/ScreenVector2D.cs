namespace GlimmerWorks.Engine.math;

/// <summary>
/// Two-dimensional vector of camera coordinates.
/// 相机坐标二维向量。
/// Unit: Screen pixels. 单位：屏幕像素。
/// </summary>
public sealed class ScreenVector2D : BaseVector<float, ScreenVector2D>
{
    public ScreenVector2D()
    {
    }

    public ScreenVector2D(float x, float y) : base(x, y)
    {
    }
}
