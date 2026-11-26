using System.Numerics;
using System.Runtime.InteropServices;

namespace GlimmerWorks.Game.Graphics.Rendering;

/// <summary>
/// <para>The vertex format used by the 2D renderer.</para>
/// <para>2D 渲染器使用的顶点格式。</para>
/// </summary>
[StructLayout(LayoutKind.Sequential)]
internal struct Vertex2D
{
    public Vector2 Position;
    public Vector2 Uv;
    public Vector4 Color;
}
