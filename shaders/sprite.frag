#version 450

// Sprite fragment shader for GlimmerWorks 2D rendering.
// Samples the bound texture and multiplies it by the per-vertex color,
// which carries tint and alpha modulation (equivalent to SDL_Renderer's
// texture color/alpha mod multiplied with the draw color).

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec4 out_color;

// SDL_GPU Vulkan backend: fragment samplers live in set 2.
layout(set = 2, binding = 0) uniform sampler2D inputTexture;

void main() {
    out_color = texture(inputTexture, in_uv) * in_color;
}
