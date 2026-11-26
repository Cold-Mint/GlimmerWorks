namespace GlimmerWorks.Engine.math;

/// <summary>
/// Generic two-dimensional vector with floating-point components.
/// 浮点二维向量。
/// </summary>
public class Vector2D : BaseVector<float, Vector2D>
{
    public Vector2D()
    {
    }

    public Vector2D(float x, float y) : base(x, y)
    {
    }
}
