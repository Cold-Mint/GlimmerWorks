namespace GlimmerWorks.Game.Ui;

/// <summary>
/// <para>
/// Fallback UI shaders used before asset packs are available, for example on the splash screen.
/// </para>
/// <para>
/// 在资产包可用之前使用的 UI 兜底着色器，例如启动画面。
/// </para>
/// </summary>
internal static class BuiltInUiShaders
{
    public const string Vertex = """
        #version 450
        layout(location = 0) in vec2 in_position;
        layout(location = 1) in vec2 in_uv;
        layout(location = 2) in vec4 in_color;
        layout(location = 0) out vec2 out_uv;
        layout(location = 1) out vec4 out_color;
        layout(set = 1, binding = 0) uniform ProjectionUniform {
            vec2 u_viewSize;
        };
        void main() {
            vec2 ndc;
            ndc.x = (in_position.x / u_viewSize.x) * 2.0 - 1.0;
            ndc.y = 1.0 - (in_position.y / u_viewSize.y) * 2.0;
            gl_Position = vec4(ndc, 0.0, 1.0);
            out_uv = in_uv;
            out_color = in_color;
        }
        """;

    public const string Fragment = """
        #version 450
        layout(location = 0) in vec2 in_uv;
        layout(location = 1) in vec4 in_color;
        layout(location = 0) out vec4 out_color;
        layout(set = 2, binding = 0) uniform sampler2D inputTexture;
        void main() {
            out_color = texture(inputTexture, in_uv) * in_color;
        }
        """;
}
