using System.Numerics;

namespace GlimmerWorks.Game.Graphics.Uniform;

/// <summary>
/// <para>
/// The shared state exposed to material uniform blocks through <c>@runtime.*</c>.
/// Scenes update it before rendering; <see cref="RuntimeUniformManager"/> reads the
/// marked public static members through reflection.
/// </para>
/// <para>
/// 通过 <c>@runtime.*</c> 暴露给材质 uniform 块的共享状态。场景在渲染前更新它，
/// <see cref="RuntimeUniformManager"/> 通过反射读取被标注的公开静态成员。
/// </para>
/// </summary>
public static class RuntimeUniformState
{
    /// <summary>
    /// <para>The per-command tint color. Exposed as <c>@runtime.tint_color</c>.</para>
    /// <para>每命令的染色颜色。通过 <c>@runtime.tint_color</c> 暴露。</para>
    /// </summary>
    [UniformValue]
    public static Vector4 TintColor { get; set; } = Vector4.One;

    /// <summary>
    /// <para>The current view size in pixels. Exposed as <c>@runtime.view_size</c>.</para>
    /// <para>当前视口尺寸（像素）。通过 <c>@runtime.view_size</c> 暴露。</para>
    /// </summary>
    [UniformValue]
    public static Vector2 ViewSize { get; set; }

    /// <summary>
    /// <para>
    /// The elapsed time in seconds. Exposed as <c>@runtime.time</c>.
    /// </para>
    /// <para>
    /// 已流逝的时间（秒）。通过 <c>@runtime.time</c> 暴露。
    /// </para>
    /// </summary>
    [UniformValue("time")]
    public static float TimeSeconds { get; set; }

    /// <summary>
    /// <para>
    /// The camera center in world pixels. Exposed as <c>@runtime.camera_position</c>.
    /// </para>
    /// <para>
    /// 世界坐标中的相机中心（像素）。通过 <c>@runtime.camera_position</c> 暴露。
    /// </para>
    /// </summary>
    [UniformValue]
    public static Vector2 CameraPosition { get; set; }

    /// <summary>
    /// <para>
    /// The camera zoom. Exposed as <c>@runtime.camera_zoom</c>.
    /// </para>
    /// <para>
    /// 相机缩放。通过 <c>@runtime.camera_zoom</c> 暴露。
    /// </para>
    /// </summary>
    [UniformValue]
    public static float CameraZoom { get; set; } = 1.0F;

    /// <summary>
    /// <para>
    /// The dimension sea level in world pixels. Exposed as <c>@runtime.sea_level</c>.
    /// </para>
    /// <para>
    /// 维度海平面（世界像素）。通过 <c>@runtime.sea_level</c> 暴露。
    /// </para>
    /// </summary>
    [UniformValue]
    public static float SeaLevel { get; set; }

    /// <summary>
    /// <para>The current global tick. Exposed as <c>@runtime.tick</c>.</para>
    /// <para>当前全局 Tick。通过 <c>@runtime.tick</c> 暴露。</para>
    /// </summary>
    [UniformValue]
    public static long Tick
    {
        get => Interlocked.Read(ref field);
        set => Interlocked.Exchange(ref field, value);
    }
}
