using System.Numerics;
using System.Runtime.InteropServices;

namespace GlimmerWorks.Game.Graphics.Rendering;

/// <summary>
/// <para>The vertex format used when drawing materials.</para>
/// <para>绘制材质时使用的顶点格式。</para>
/// </summary>
[StructLayout(LayoutKind.Sequential)]
internal struct MaterialVertex2D
{
    public Vector2 Position;
    public Vector2 Uv;
}
