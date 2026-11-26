using System.Drawing;

namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>A GPU texture created by the Myra texture manager.</para>
/// <para>由 Myra 纹理管理器创建的 GPU 纹理。</para>
/// </summary>
internal sealed class Sdl3UiTexture(IntPtr handle, int width, int height)
{
    public IntPtr Handle { get; } = handle;
    public Point Size => new(width, height);
}