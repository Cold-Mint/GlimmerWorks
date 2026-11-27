using SDL3;

namespace GlimmerWorks.Game.Graphics.Vertex;

public record struct VertexFormat(SDL.GPUVertexElementFormat Format, int Size)
{
}