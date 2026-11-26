namespace GlimmerWorks.Engine.Mods;

public struct Color(int r, int g, int b, int a = 255) : IEquatable<Color>
{
    public int R { get; set; } = r;
    public int G { get; set; } = g;
    public int B { get; set; } = b;
    public int A { get; set; } = a;

    public readonly bool Equals(Color other) =>
        R == other.R && G == other.G && B == other.B && A == other.A;

    public readonly override bool Equals(object? obj) => obj is Color other && Equals(other);

    public readonly override int GetHashCode() => HashCode.Combine(R, G, B, A);

    public static bool operator ==(Color left, Color right) => left.Equals(right);

    public static bool operator !=(Color left, Color right) => !left.Equals(right);
}
