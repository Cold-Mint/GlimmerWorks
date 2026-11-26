using SDL3;

namespace GlimmerWorks.Game.Scene;

public readonly record struct SceneFrameContext(
    IntPtr Device,
    IntPtr CommandBuffer,
    IntPtr SwapchainTexture,
    uint Width,
    uint Height,
    SDL.GPUTextureFormat SwapchainFormat);