using System.Numerics;

namespace GlimmerWorks.Engine.math;

/// <summary>
/// Common base for the two-dimensional vector types.
/// 二维向量的公共基类。
/// </summary>
/// <typeparam name="T">Component type (float, int, uint...). 分量类型。</typeparam>
/// <typeparam name="TSelf">The concrete vector type (CRTP). 具体的向量类型。</typeparam>
public abstract class BaseVector<T, TSelf> : IEquatable<TSelf>
    where T : INumber<T>
    where TSelf : BaseVector<T, TSelf>, new()
{
    /// <summary>
    /// X component. X 分量。
    /// </summary>
    public T X { get; set; } = T.Zero;

    /// <summary>
    /// Y component. Y 分量。
    /// </summary>
    public T Y { get; set; } = T.Zero;

    protected BaseVector()
    {
    }

    protected BaseVector(T x, T y)
    {
        X = x;
        Y = y;
    }

    /// <summary>
    /// Vector addition. 向量加法。
    /// </summary>
    public static TSelf operator +(BaseVector<T, TSelf> lhs, BaseVector<T, TSelf> rhs) =>
        new() { X = lhs.X + rhs.X, Y = lhs.Y + rhs.Y };

    /// <summary>
    /// Vector subtraction. 向量减法。
    /// </summary>
    public static TSelf operator -(BaseVector<T, TSelf> lhs, BaseVector<T, TSelf> rhs) =>
        new() { X = lhs.X - rhs.X, Y = lhs.Y - rhs.Y };

    /// <summary>
    /// Vector multiplication by a scalar. 向量乘标量。
    /// </summary>
    public static TSelf operator *(BaseVector<T, TSelf> lhs, float scalar) =>
        new()
        {
            X = T.CreateChecked(float.CreateChecked(lhs.X) * scalar),
            Y = T.CreateChecked(float.CreateChecked(lhs.Y) * scalar)
        };

    /// <summary>
    /// Vector multiplication by a scalar. 向量乘标量。
    /// </summary>
    public static TSelf operator *(float scalar, BaseVector<T, TSelf> rhs) => rhs * scalar;

    /// <summary>
    /// Vector division by a scalar. 向量除以标量。
    /// </summary>
    public static TSelf operator /(BaseVector<T, TSelf> lhs, float scalar) =>
        new()
        {
            X = T.CreateChecked(float.CreateChecked(lhs.X) / scalar),
            Y = T.CreateChecked(float.CreateChecked(lhs.Y) / scalar)
        };

    /// <summary>
    /// Dot product. 点积。
    /// </summary>
    public T Dot(BaseVector<T, TSelf> rhs) => X * rhs.X + Y * rhs.Y;

    /// <summary>
    /// Length of the vector. 获取向量长度。
    /// </summary>
    public T Length()
    {
        T lengthSquared = LengthSquared();
        return typeof(T) == typeof(float)
            ? (T)(object)MathF.Sqrt((float)(object)lengthSquared)
            : T.CreateChecked(Math.Sqrt(double.CreateChecked(lengthSquared)));
    }

    /// <summary>
    /// Squared length of the vector (no square root). 获取向量长度的平方（不开方）。
    /// </summary>
    public T LengthSquared() => X * X + Y * Y;

    /// <summary>
    /// Distance to another vector. 获取到另一个向量的距离。
    /// </summary>
    public T Distance(BaseVector<T, TSelf> rhs) => (this - rhs).Length();

    /// <summary>
    /// Squared distance to another vector (no square root). 获取到另一个向量的距离的平方（不开方）。
    /// </summary>
    public T DistanceSquared(BaseVector<T, TSelf> rhs) => (this - rhs).LengthSquared();

    /// <summary>
    /// Normalized vector. 向量归一化。
    /// </summary>
    public TSelf Normalized()
    {
        T length = Length();
        bool isZero = T.IsInteger(length)
            ? length < T.One
            : double.CreateChecked(length) < 0.0001;

        if (isZero)
        {
            return new TSelf();
        }

        return new TSelf { X = X / length, Y = Y / length };
    }

    /// <summary>
    /// Angle from this vector to another one, in degrees (0-360).
    /// 计算到另一个向量之间的角度，0-360 度。
    /// </summary>
    public float ToAngle(BaseVector<T, TSelf> to)
    {
        float dx = float.CreateChecked(to.X - X);
        float dy = float.CreateChecked(to.Y - Y);
        float angle = MathF.Atan2(dy, dx) * 180.0F / MathF.PI;
        return angle < 0 ? angle + 360.0F : angle;
    }

    /// <summary>
    /// Pack the vector into a fingerprint. 将向量打包为指纹。
    /// </summary>
    public ulong GetFingerprint() => (ulong)ToBits(X) << 32 | ToBits(Y);

    /// <summary>
    /// Restore the vector from a fingerprint. 从指纹还原向量。
    /// </summary>
    public void FromFingerprint(ulong fingerprint)
    {
        X = FromBits((uint)(fingerprint >> 32));
        Y = FromBits((uint)fingerprint);
    }

    private static uint ToBits(T value)
    {
        if (typeof(T) == typeof(float))
        {
            return BitConverter.SingleToUInt32Bits((float)(object)value);
        }

        if (typeof(T) == typeof(int))
        {
            return unchecked((uint)(int)(object)value);
        }

        if (typeof(T) == typeof(uint))
        {
            return (uint)(object)value;
        }

        throw new NotSupportedException($"Fingerprints are not supported for {typeof(T)}.");
    }

    private static T FromBits(uint bits)
    {
        object value;
        if (typeof(T) == typeof(float))
        {
            value = BitConverter.UInt32BitsToSingle(bits);
        }
        else if (typeof(T) == typeof(int))
        {
            value = unchecked((int)bits);
        }
        else if (typeof(T) == typeof(uint))
        {
            value = bits;
        }
        else
        {
            throw new NotSupportedException($"Fingerprints are not supported for {typeof(T)}.");
        }

        return (T)value;
    }

    public bool Equals(TSelf? other) => other is not null && X == other.X && Y == other.Y;

    public override bool Equals(object? obj) => obj is TSelf other && Equals(other);

    public override int GetHashCode() => HashCode.Combine(X, Y);

    public static bool operator ==(BaseVector<T, TSelf>? lhs, BaseVector<T, TSelf>? rhs) =>
        lhs is null ? rhs is null : rhs is not null && lhs.X == rhs.X && lhs.Y == rhs.Y;

    public static bool operator !=(BaseVector<T, TSelf>? lhs, BaseVector<T, TSelf>? rhs) => !(lhs == rhs);
}
