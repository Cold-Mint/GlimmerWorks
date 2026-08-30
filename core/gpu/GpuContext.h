/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once

#include <memory>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>


namespace glimmer {
    /**
     * GpuContext
     * GPU 上下文
     *
     * Owns the SDL_GPUDevice, the claimed window swapchain and the shared
     * samplers, and provides texture creation / readback utilities.
     * This class replaces SDL_Renderer as the central graphics object.
     * 拥有 SDL_GPUDevice、已声明的窗口交换链和共享采样器，
     * 并提供纹理创建/回读实用功能。
     * 该类替代 SDL_Renderer 成为核心图形对象。
     *
     * Lifetime: created once during application init (Init), destroyed at exit
     * (Shutdown or destructor). All GpuTexture objects must be destroyed before
     * Shutdown is called.
     * 生命周期：应用初始化时创建一次（Init），退出时销毁（Shutdown 或析构）。
     * 所有 GpuTexture 必须在 Shutdown 调用前销毁。
     */
    class GpuContext {
        SDL_GPUDevice *device_ = nullptr;
        SDL_Window *window_ = nullptr;
        SDL_GPUSampler *nearestSampler_ = nullptr;
        SDL_GPUSampler *linearSampler_ = nullptr;

    public:
        GpuContext() = default;

        ~GpuContext();

        GpuContext(const GpuContext &) = delete;

        GpuContext &operator=(const GpuContext &) = delete;

        /**
         * Create the GPU device (SPIRV/Vulkan), claim the window for the
         * swapchain and create the shared samplers.
         * 创建 GPU 设备（SPIRV/Vulkan），为交换链声明窗口，并创建共享采样器。
         * @param window window 渲染目标窗口
         * @param vSync vSync true 使用垂直同步呈现模式，false 使用立即呈现模式
         * @return true on success, false on failure (error is logged).
         * 成功返回 true，失败返回 false（错误会记录日志）。
         */
        bool Init(SDL_Window *window, bool vSync);

        /**
         * Release the samplers, release the window from the device and destroy
         * the GPU device. Safe to call multiple times.
         * 释放采样器、从设备释放窗口并销毁 GPU 设备。可安全地多次调用。
         */
        void Shutdown();

        /**
         * @return The raw SDL_GPUDevice handle.
         * 原始 SDL_GPUDevice 句柄。
         */
        [[nodiscard]] SDL_GPUDevice *GetDevice() const;

        /**
         * @return The window claimed by this context.
         * 此上下文声明的窗口。
         */
        [[nodiscard]] SDL_Window *GetWindow() const;

        /**
         * @return The swapchain texture format of the claimed window.
         * 已声明窗口的交换链纹理格式。
         */
        [[nodiscard]] SDL_GPUTextureFormat GetSwapchainFormat() const;

        /**
         * @return Shared nearest-filter sampler (pixel-art default).
         * 共享最近邻过滤采样器（像素风默认）。
         */
        [[nodiscard]] SDL_GPUSampler *GetNearestSampler() const;

        /**
         * @return Shared linear-filter sampler.
         * 共享线性过滤采样器。
         */
        [[nodiscard]] SDL_GPUSampler *GetLinearSampler() const;

        /**
         * Create an offscreen render-target texture (COLOR_TARGET | SAMPLER).
         * Used for layer compositing (game/ui layers) and full-screen
         * post-processing.
         * 创建离屏渲染目标纹理（COLOR_TARGET | SAMPLER）。
         * 用于分层合成（game/ui 层）与全屏后处理。
         * @param width width 纹理宽度（像素，必须 > 0）
         * @param height height 纹理高度（像素，必须 > 0）
         * @param format format 纹理格式（通常与交换链格式一致）
         * @return A new GpuTexture (caller owns it), nullptr on failure.
         * 新的 GpuTexture（调用方拥有所有权），失败返回 nullptr。
         */
        [[nodiscard]] GpuTexture *CreateTargetTexture(Uint32 width, Uint32 height,
                                                      SDL_GPUTextureFormat format) const;

        /**
         * Create an empty sampled-only texture (SAMPLER usage, no pixel data
         * uploaded). Intended for dynamic textures whose contents are
         * refreshed from CPU memory every frame (e.g. the tile light map),
         * the actual upload happens through a copy pass on the frame
         * command buffer.
         * 创建空的仅采样纹理（SAMPLER 用途，不上传像素数据）。
         * 用于每帧从 CPU 内存刷新内容的动态纹理（例如瓦片光照贴图），
         * 实际上传通过帧命令缓冲上的拷贝通道完成。
         * @param width width 纹理宽度（像素，必须 > 0）
         * @param height height 纹理高度（像素，必须 > 0）
         * @param format format 纹理格式（例如 SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM）
         * @return A new GpuTexture (caller owns it), nullptr on failure.
         * 新的 GpuTexture（调用方拥有所有权），失败返回 nullptr。
         */
        [[nodiscard]] GpuTexture *CreateSampledTexture(Uint32 width, Uint32 height,
                                                       SDL_GPUTextureFormat format) const;

        /**
         * Download the full contents of a GPU texture into a new SDL_Surface.
         * Blocks until the GPU has finished the copy.
         * 将 GPU 纹理的全部内容下载到新的 SDL_Surface。
         * 阻塞直到 GPU 完成拷贝。
         * @param texture texture 要回读的纹理
         * @param width width 纹理宽度（像素）
         * @param height height 纹理高度（像素）
         * @param pixelFormat pixelFormat 目标表面的 SDL 像素格式
         * （必须与纹理的字节布局一致，例如 R8G8B8A8_UNORM 对应 SDL_PIXELFORMAT_ABGR8888）
         * @return A new SDL_Surface (caller destroys it), nullptr on failure.
         * 新的 SDL_Surface（调用方负责销毁），失败返回 nullptr。
         */
        [[nodiscard]] SDL_Surface *ReadbackTexture(SDL_GPUTexture *texture, Uint32 width, Uint32 height,
                                                   SDL_PixelFormat pixelFormat) const;
    };
}
