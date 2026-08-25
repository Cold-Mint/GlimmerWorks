/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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

#include "GpuContext.h"
#include "GpuTexture.h"
#include "RenderCommand.h"
#include "SDL3/SDL_pixels.h"

namespace glimmer {
    class RenderQueue;
    class ResourcePackManager;
    struct Mods;

    /**
     * GpuRenderer
     * GPU 渲染器
     *
     * Frame-level renderer built on SDL_GPU. It owns the graphics pipelines,
     * the dynamic vertex buffer and the offscreen layer textures, and draws
     * the contents of a RenderQueue (see FlushQueue). It replaces the old
     * immediate-mode SpriteRenderer: instead of accumulating vertices on
     * every draw call, game systems record RenderCommands into a RenderQueue
     * and the renderer consumes the whole sorted queue once per frame.
     * 基于 SDL_GPU 的帧级渲染器。它持有图形管线、动态顶点缓冲和离屏层纹理，
     * 并绘制 RenderQueue 的内容（见 FlushQueue）。它替代了旧的立即模式
     * SpriteRenderer：游戏系统不再在每次绘制调用时累积顶点，而是把
     * RenderCommand 记录进 RenderQueue，由渲染器每帧一次性消费整个
     * 排好序的队列。
     *
     * Layered rendering (shaders are loaded from the enabled resource packs,
     * directory shaders/@core/):
     * 分层渲染（着色器从已启用的材质包加载，目录为 shaders/@core/）：
     *   1. All queued commands are drawn into the offscreen game layer,
     *      layer by layer (see RenderLayer).
     *      队列中的所有命令逐层（见 RenderLayer）绘制进离屏 game 层。
     *   2. RmlUi draws into the offscreen ui layer (see GetUiTargetTexture).
     *      RmlUi 绘制到离屏 ui 层（见 GetUiTargetTexture）。
     *   3. CompositeToSwapchain() applies the "game" shader to the game layer
     *      and the "ui" shader to the ui layer, blends both into the composite
     *      layer, then applies the "global" shader to produce the final image
     *      on the swapchain.
     *      CompositeToSwapchain() 对 game 层应用 "game" 着色器、对 ui 层应用
     *      "ui" 着色器，二者混合进合成层，再对其应用 "global" 着色器输出到交换链。
     *
     * Typical frame flow (driven by AppRenderer):
     * 典型帧流程（由 AppRenderer 驱动）：
     *   BeginFrame(window)          -> acquire swapchain, clear game/ui layers
     *   (systems submit commands into the RenderQueue)
     *   （各系统向 RenderQueue 提交命令）
     *   FlushQueue(queue)           -> sort, upload and draw into the game layer
     *   （排序、上传并绘制进 game 层）
     *   (RmlUi renders into the ui layer on the same command buffer)
     *   （RmlUi 在同一命令缓冲上渲染进 ui 层）
     *   CompositeToSwapchain()      -> game/ui/global shader passes
     *   SubmitFrame()               -> submit the command buffer
     */
    class GpuRenderer {
        /**
         * A contiguous run of vertices sharing the same texture.
         * 共享同一纹理的一段连续顶点。
         */
        struct DrawRun {
            SDL_GPUTexture *texture = nullptr;
            Uint32 firstVertex = 0;
            Uint32 vertexCount = 0;
        };

        static constexpr Uint32 INITIAL_VERTEX_CAPACITY = 65536;

        GpuContext *gpuContext_ = nullptr;
        SDL_GPUDevice *device_ = nullptr;
        SDL_GPUGraphicsPipeline *pipeline_ = nullptr;
        SDL_GPUGraphicsPipeline *gamePipeline_ = nullptr;
        SDL_GPUGraphicsPipeline *uiPipeline_ = nullptr;
        SDL_GPUGraphicsPipeline *globalPipeline_ = nullptr;
        std::unique_ptr<GpuTexture> whiteTexture_ = nullptr;

        std::unique_ptr<GpuTexture> gameLayerTexture_ = nullptr;
        std::unique_ptr<GpuTexture> uiLayerTexture_ = nullptr;
        std::unique_ptr<GpuTexture> compositeLayerTexture_ = nullptr;

        SDL_GPUBuffer *vertexBuffer_ = nullptr;
        Uint32 vertexBufferCapacity_ = 0;
        SDL_GPUBuffer *unitQuadBuffer_ = nullptr;

        SDL_GPUCommandBuffer *commandBuffer_ = nullptr;
        SDL_GPUTexture *swapchainTexture_ = nullptr;
        Uint32 swapchainWidth_ = 0;
        Uint32 swapchainHeight_ = 0;
        SDL_GPURenderPass *renderPass_ = nullptr;
        SDL_GPUTexture *currentTarget_ = nullptr;
        bool frameActive_ = false;

        /**
         * Clear color of the game layer (the world background).
         * game 层的清屏颜色（世界背景色）。
         */
        SDL_Color clearColor_ = {0, 0, 0, 255};

        /**
         * Create a sprite-style graphics pipeline (shared vertex input layout).
         * 创建精灵风格的图形管线（共享顶点输入布局）。
         * @param vertexShader vertexShader 顶点着色器
         * @param fragmentShader fragmentShader 片元着色器
         * @param enableBlend enableBlend 是否启用标准 alpha 混合
         * @return The pipeline on success, nullptr on failure.
         * 成功返回管线，失败返回 nullptr。
         */
        SDL_GPUGraphicsPipeline *CreateSpritePipeline(SDL_GPUShader *vertexShader, SDL_GPUShader *fragmentShader,
                                                      bool enableBlend) const;

        /**
         * Recreate the offscreen layer textures when the swapchain size changed.
         * 交换链尺寸变化时重建离屏层纹理。
         * @return true if all layer textures are valid.
         * 所有层纹理有效时返回 true。
         */
        bool EnsureLayerTextures();

        /**
         * Begin a render pass on the current target if none is active.
         * 若没有活动通道，则在当前目标上开始一个渲染通道。
         * @param clear clear 为 true 时用当前 clearColor 清屏，否则保留已有内容
         */
        void EnsureRenderPass(bool clear);

        /**
         * End the active render pass (if any).
         * 结束当前活动的渲染通道（若有）。
         */
        void EndActivePass();

        /**
         * Draw a full-screen quad sampling a layer texture with a layer pipeline.
         * Must be called inside an active render pass.
         * 用分层管线绘制一个采样层纹理的全屏四边形。必须在活动渲染通道内调用。
         * @param pipeline pipeline 使用的管线
         * @param source source 源层纹理
         */
        void DrawLayerQuad(SDL_GPUGraphicsPipeline *pipeline, const GpuTexture *source);

    public:
        GpuRenderer() = default;

        ~GpuRenderer();

        GpuRenderer(const GpuRenderer &) = delete;

        GpuRenderer &operator=(const GpuRenderer &) = delete;

        /**
         * Compile the shaders (loaded from the enabled resource packs, with
         * embedded pass-through fallbacks), create the graphics pipelines, the
         * built-in white texture and the full-screen unit quad buffer.
         * 编译着色器（从已启用的材质包加载，缺失时使用内嵌的 pass-through
         * 兜底），创建图形管线、内置白色纹理和全屏单位四边形缓冲。
         * @param gpuContext gpuContext 已初始化的 GPU 上下文
         * @param resourcePackManager resourcePackManager 材质包管理器（提供着色器源码）
         * @param mods mods 模组配置（决定启用哪些材质包）
         * @return true on success, false on failure (error is logged).
         * 成功返回 true，失败返回 false（错误会记录日志）。
         */
        bool Init(GpuContext *gpuContext, ResourcePackManager *resourcePackManager, const Mods &mods);

        /**
         * Release the pipelines, buffers, white texture and layer textures.
         * 释放管线、缓冲、白色纹理和层纹理。
         */
        void Shutdown();

        /**
         * Begin a new frame: acquire a command buffer and the swapchain
         * texture, (re)create the layer textures if needed, clear the ui layer
         * and begin the game layer pass with the clear color.
         * 开始新帧：获取命令缓冲和交换链纹理，必要时重建层纹理，
         * 清除 ui 层并用清屏颜色开始 game 层通道。
         * @param window window 目标窗口
         * @return true if a swapchain texture was acquired and drawing is
         * possible, false if the window is minimized/unavailable (frame
         * functions become no-ops but must still be called).
         * 成功获取交换链纹理返回 true；窗口最小化等情况返回 false
         * （此时帧函数为空操作，但仍必须被调用）。
         */
        bool BeginFrame(SDL_Window *window);

        /**
         * Sort the render queue (layer ascending, then depth ascending),
         * expand every command into batch vertices, upload them and draw them
         * into the game layer, then end the game layer render pass.
         * 对渲染队列排序（层升序，然后 depth 升序），把每个命令展开为批处理
         * 顶点，上传并绘制进 game 层，然后结束 game 层渲染通道。
         * @param queue queue 本帧填充完毕的渲染队列
         */
        void FlushQueue(RenderQueue &queue);

        /**
         * @return The offscreen ui layer texture that RmlUi must render into.
         * RmlUi 必须渲染到的离屏 ui 层纹理。
         */
        [[nodiscard]] GpuTexture *GetUiTargetTexture() const;

        /**
         * Apply the game shader to the game layer and the ui shader to the ui
         * layer, blend both into the composite layer, then apply the global
         * shader to draw the final image onto the swapchain. Records the
         * passes but does NOT submit the command buffer.
         * 对 game 层应用 game 着色器、对 ui 层应用 ui 着色器并混合进合成层，
         * 再对其应用 global 着色器把最终画面绘制到交换链。
         * 仅记录通道，不提交命令缓冲。
         */
        void CompositeToSwapchain();

        /**
         * Submit the frame command buffer (presents the swapchain texture).
         * 提交帧命令缓冲（呈现交换链纹理）。
         * @return the submitted command buffer's submit result.
         * 提交是否成功。
         */
        bool SubmitFrame();

        /**
         * @return The current frame command buffer (for RmlUi / copy passes).
         * 当前帧命令缓冲（供 RmlUi / 拷贝通道使用）。
         */
        [[nodiscard]] SDL_GPUCommandBuffer *GetCommandBuffer() const;

        /**
         * @return The current swapchain texture (for screenshots).
         * 当前交换链纹理（供截图使用）。
         */
        [[nodiscard]] SDL_GPUTexture *GetSwapchainTexture() const;

        [[nodiscard]] Uint32 GetSwapchainWidth() const;

        [[nodiscard]] Uint32 GetSwapchainHeight() const;

        /**
         * Set the clear color of the game layer (the world background color
         * visible where nothing is drawn).
         * 设置 game 层的清屏颜色（未绘制任何内容处可见的世界背景色）。
         */
        void SetClearColor(SDL_Color color);

        /**
         * @return The current game layer clear color.
         * 当前 game 层清屏颜色。
         */
        [[nodiscard]] SDL_Color GetClearColor() const;
    };
}
