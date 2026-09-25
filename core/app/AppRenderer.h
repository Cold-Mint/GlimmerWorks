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
#include <vector>

#include <SDL3/SDL_gpu.h>

#include "core/context/AppContext.h"
#include "core/gpu/PendingScreenshot.h"
#include "core/gpu/RenderQueue.h"

namespace glimmer {
    class ResourceLocator;
    class SceneManager;
    class RmlContext;
    class IPass;
    class LightingPass;

    /**
     * AppRenderer
     * 应用渲染器
     *
     * Drives one frame of rendering: clears the per-frame RenderQueue, lets
     * the scenes/overlays submit their commands, then delegates the actual GPU
     * work to an ordered sequence of Pass objects (scene, clear, lighting, UI)
     * before submitting the frame.
     * 驱动一帧的渲染：清空每帧的 RenderQueue，让场景/覆盖层提交命令，然后把
     * 实际 GPU 工作委托给有序的 Pass 序列（场景、清屏、光照、UI），
     * 最后提交这一帧。
     */
    class AppRenderer {
        AppContext *appContext_ = nullptr;
        RenderQueue renderQueue_;
        SDL_GPUDevice *device_ = nullptr;
        SDL_Window *window_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;
        SceneManager *sceneManager_ = nullptr;
        std::vector<std::unique_ptr<IPass> > passes_;
#if  !defined(NDEBUG)
        LightingPass *lightingPass_ = nullptr;
#endif

        //When a screenshot is requested, passes render into this texture instead
        //of the swapchain. It is then blitted to the swapchain and downloaded.
        //截图请求时，各 pass 先渲染到此纹理而非交换链，然后再 blit 到交换链并下载。
        SDL_GPUTexture *screenshotTexture_ = nullptr;
        Uint32 screenshotTextureWidth_ = 0;
        Uint32 screenshotTextureHeight_ = 0;

        void RenderOverlays();

        /**
         * Create or recreate the screenshot target texture so it matches the
         * requested size and swapchain format.
         * 创建或重建截图目标纹理，使其与请求的尺寸和交换链格式匹配。
         */
        void EnsureScreenshotTexture(Uint32 width, Uint32 height);

        /**
         * Blit the screenshot target texture to the real swapchain texture.
         * 将截图目标纹理 Blit 到真正的交换链纹理。
         */
        static void BlitScreenshotToSwapChain(SDL_GPUCommandBuffer *commandBuffer,
                                              SDL_GPUTexture *source,
                                              SDL_GPUTexture *destination,
                                              Uint32 width, Uint32 height);

        /**
         * Download the current scene image and save it to the path stored in
         * the pending screenshot request. This function submits and waits on the
         * command buffer internally, so RenderFrame must skip the normal submit
         * when a screenshot was processed.
         * 下载当前场景图像并保存到截图请求指定的路径。此函数会内部提交并等待
         * 命令缓冲，因此处理截图后 RenderFrame 必须跳过正常提交。
         */
        bool SaveScreenshot(const PendingScreenshot &pendingScreenshot, const struct RenderFrameContext &ctx);

    public:
        explicit AppRenderer(AppContext *appContext);

        ~AppRenderer();

        /**
         * RenderFrame
         * 渲染一帧。
         * @param windowWidth
         * @param windowHeight
         */
        void RenderFrame(int windowWidth, int windowHeight);
    };
}
