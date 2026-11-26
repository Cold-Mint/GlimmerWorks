namespace GlimmerWorks.Game.Graphics;

/// <summary>
/// <para>The vertex formats supported when creating graphics pipelines.</para>
/// <para>创建图形管线时支持的顶点格式。</para>
/// </summary>
public enum VertexLayout
{
    /// <summary>
    /// <para>Position (vec2) + UV (vec2), 16 bytes per vertex.</para>
    /// <para>位置 (vec2) + UV (vec2)，每顶点 16 字节。</para>
    /// </summary>
    PositionTexture,

    /// <summary>
    /// <para>Position (vec2) + UV (vec2) + color (vec4), 32 bytes per vertex.</para>
    /// <para>位置 (vec2) + UV (vec2) + 颜色 (vec4)，每顶点 32 字节。</para>
    /// </summary>
    PositionTextureColor
}
