namespace GlimmerWorks.Engine.Mods;

public struct Color(int r, int g, int b, int a = 255)
{
    public int R { get; set; } = r;
    public int G { get; set; } = g;
    public int B { get; set; } = b;
    public int A { get; set; } = a;
}