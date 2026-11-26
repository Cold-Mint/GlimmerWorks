using System.Buffers.Binary;
using System.Numerics;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>A runtime uniform value read from a <see cref="UniformValueAttribute"/> member.</para>
/// <para>从 <see cref="UniformValueAttribute"/> 成员读取的运行时 uniform 值。</para>
/// </summary>
public readonly struct UniformValue
{
    private readonly float[] _components;

    /// <summary>
    /// <para>Whether the value should be written as an integer.</para>
    /// <para>是否以整数形式写入。</para>
    /// </summary>
    public bool IsInteger { get; }

    /// <summary>
    /// <para>The float components of the value.</para>
    /// <para>值的浮点分量。</para>
    /// </summary>
    public ReadOnlySpan<float> Components => _components ?? EmptyComponents;

    private static readonly float[] EmptyComponents = [];

    private UniformValue(float[] components, bool isInteger)
    {
        _components = components;
        IsInteger = isInteger;
    }

    public static UniformValue FromFloat(float value) => new([value], false);

    public static UniformValue FromInt(int value) => new([value], true);

    public static UniformValue FromUInt(uint value) => new([value], true);

    public static UniformValue FromBool(bool value) => new([value ? 1f : 0f], true);

    public static UniformValue FromVector2(Vector2 value) => new([value.X, value.Y], false);

    public static UniformValue FromVector3(Vector3 value) => new([value.X, value.Y, value.Z], false);

    public static UniformValue FromVector4(Vector4 value) => new([value.X, value.Y, value.Z, value.W], false);

    public static UniformValue FromMatrix4x4(Matrix4x4 value) => new(
    [
        // GLSL mat4 is column-major by default, so upload the columns of the
        // row-major System.Numerics matrix.
        // GLSL 的 mat4 默认是列主序，因此按列上传行主序的 System.Numerics 矩阵。
        value.M11, value.M21, value.M31, value.M41,
        value.M12, value.M22, value.M32, value.M42,
        value.M13, value.M23, value.M33, value.M43,
        value.M14, value.M24, value.M34, value.M44
    ], false);

    /// <summary>
    /// <para>
    /// Convert an object returned by a <see cref="UniformValueAttribute"/> member
    /// into a uniform value.
    /// </para>
    /// <para>
    /// 把 <see cref="UniformValueAttribute"/> 成员返回的对象转换为 uniform 值。
    /// </para>
    /// </summary>
    /// <param name="source"></param>
    /// <param name="value"></param>
    /// <returns>
    ///<para><c>true</c> when the source type is supported.</para>
    ///<para>源类型受支持时返回 <c>true</c>。</para>
    /// </returns>
    public static bool TryFromObject(object? source, out UniformValue value)
    {
        switch (source)
        {
            case float floatValue:
                value = FromFloat(floatValue);
                return true;
            case double doubleValue:
                value = FromFloat((float)doubleValue);
                return true;
            case int intValue:
                value = FromInt(intValue);
                return true;
            case uint uintValue:
                value = FromUInt(uintValue);
                return true;
            case long longValue:
                value = FromFloat(longValue);
                return true;
            case ulong ulongValue:
                value = FromFloat(ulongValue);
                return true;
            case bool boolValue:
                value = FromBool(boolValue);
                return true;
            case Vector2 vector2:
                value = FromVector2(vector2);
                return true;
            case Vector3 vector3:
                value = FromVector3(vector3);
                return true;
            case Vector4 vector4:
                value = FromVector4(vector4);
                return true;
            case Matrix4x4 matrix4X4:
                value = FromMatrix4x4(matrix4X4);
                return true;
            default:
                value = default;
                return false;
        }
    }

    /// <summary>
    /// <para>Create a value from TOML numbers.</para>
    /// <para>从 TOML 数字创建值。</para>
    /// </summary>
    internal static UniformValue FromNumbers(IReadOnlyList<double> numbers) =>
        new([.. numbers.Select(number => (float)number)], false);

    /// <summary>
    /// <para>Write this value into a uniform block buffer.</para>
    /// <para>把值写入 uniform block 缓冲区。</para>
    /// </summary>
    /// <param name="destination">
    ///<para>The destination buffer.</para>
    ///<para>目标缓冲区。</para>
    /// </param>
    /// <param name="offset">
    ///<para>The byte offset of the member.</para>
    ///<para>成员的字节偏移。</para>
    /// </param>
    /// <param name="info">
    ///<para>The target member layout.</para>
    ///<para>目标成员布局。</para>
    /// </param>
    internal void WriteTo(byte[] destination, int offset, UniformTypeInfo info)
    {
        var components = Components;
        for (var i = 0; i < info.ComponentCount; i++)
        {
            // A single component is broadcast to every component of a vector.
            // 单分量会广播到向量的每个分量。
            var component = components.Length switch
            {
                0 => 0f,
                1 => components[0],
                _ => i < components.Length ? components[i] : 0f
            };

            var target = offset + i * sizeof(float);
            switch (info.Kind)
            {
                case UniformValueKind.Int:
                    BinaryPrimitives.WriteInt32LittleEndian(destination.AsSpan(target, sizeof(int)),
                        (int)component);
                    break;
                case UniformValueKind.UInt:
                    BinaryPrimitives.WriteUInt32LittleEndian(destination.AsSpan(target, sizeof(uint)),
                        (uint)component);
                    break;
                case UniformValueKind.Bool:
                    BinaryPrimitives.WriteInt32LittleEndian(destination.AsSpan(target, sizeof(int)),
                        component != 0f ? 1 : 0);
                    break;
                default:
                    BinaryPrimitives.WriteSingleLittleEndian(destination.AsSpan(target, sizeof(float)),
                        component);
                    break;
            }
        }
    }
}