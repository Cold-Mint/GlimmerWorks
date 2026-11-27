namespace GlimmerWorks.Game.Graphics.Pipeline;

/// <summary>
/// <para>The blend modes a pipeline can use.</para>
/// <para>管线可使用的混合模式。</para>
/// </summary>
public enum BlendMode
{
    /// <summary>
    /// <para>No blending: the fragment color overwrites the target.</para>
    /// <para>不混合：片元颜色覆盖目标。</para>
    /// </summary>
    None = 0,

    /// <summary>
    /// <para>Straight alpha blending: src * srcAlpha + dst * (1 - srcAlpha).</para>
    /// <para>常规 Alpha 混合：src * srcAlpha + dst * (1 - srcAlpha)。</para>
    /// </summary>
    Alpha = 1,

    /// <summary>
    /// <para>Additive blending: src * srcAlpha + dst.</para>
    /// <para>加性混合：src * srcAlpha + dst。</para>
    /// </summary>
    Additive = 2
}