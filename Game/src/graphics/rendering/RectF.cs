using System.Numerics;

namespace GlimmerWorks.Game.Graphics.Rendering;

/// <summary>
/// <para>A rectangle in pixel coordinates.</para>
/// <para>像素坐标中的矩形。</para>
/// </summary>
public readonly record struct RectF(float X, float Y, float Width, float Height)
{
    public float Left => X;

    public float Top => Y;

    public float Right => X + Width;

    public float Bottom => Y + Height;

    public Vector2 Position => new(X, Y);

    public Vector2 Size => new(Width, Height);

    public static RectF FromBounds(Vector2 position, Vector2 size)
    {
        return new RectF(position.X, position.Y, size.X, size.Y);
    }
}
