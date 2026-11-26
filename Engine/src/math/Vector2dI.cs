namespace GlimmerWorks.Engine.math;

/// <summary>
/// Generic two-dimensional vector with signed integer components.
/// 有符号整数二维向量。
/// </summary>
public class Vector2DI : BaseVector<int, Vector2DI>
{
    public Vector2DI()
    {
    }

    public Vector2DI(int x, int y) : base(x, y)
    {
    }
}
