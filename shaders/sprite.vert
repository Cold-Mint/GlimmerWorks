#version 450

// Sprite vertex shader for GlimmerWorks 2D rendering.
// Input positions are in pixel coordinates with a top-left origin (+Y down),
// matching the coordinate system used by all game code.
// SDL_GPU NDC space has a bottom-left origin, so Y is flipped here.

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec4 out_color;

// SDL_GPU Vulkan backend: vertex uniform buffers live in set 1.
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
