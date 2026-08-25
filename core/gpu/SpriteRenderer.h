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

#include "GpuContext.h"
#include "GpuTexture.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"

namespace glimmer {
    class ResourcePackManager;
    struct Mods;

    /**
     * Flip flags for DrawTextureRotated (replaces SDL_FlipMode).
     * DrawTextureRotated 使用的翻转标志（替代 SDL_FlipMode）。
     */
    enum FlipFlags : Uint8 {
        FLIP_NONE = 0,
        FLIP_HORIZONTAL = 1,
        FLIP_VERTICAL = 2
    };

    /**
     * SpriteRenderer
     * 精灵（纹理）绘制工具
     *
     * Immediate-mode style 2D texture renderer built on SDL_GPU. It batches
     * quads grouped by texture into a single dynamic vertex buffer and draws
     * them with the sprite pipeline.
     * 基于 SDL_GPU 的立即模式风格 2D 纹理渲染器。它将按纹理分组的四边形
     * 批处理进单个动态顶点缓冲，并使用精灵管线绘制。
     *
     * Layered rendering (shaders are loaded from the enabled resource packs,
     * directory shaders/@core/):
     * 分层渲染（着色器从已启用的材质包加载，目录为 shaders/@core/）：
     *   1. All sprite/geometry draws go into the offscreen game layer.
     *      所有精灵/几何绘制进入离屏 game 层。
     *   2. RmlUi draws into the offscreen ui layer (see GetUiTargetTexture).
     *      RmlUi 绘制到离屏 ui 层（见 GetUiTargetTexture）。
     *   3. CompositeToSwapchain() applies the "game" shader to the game layer
     *      and the "ui" shader to the ui layer, blends both into the composite
     *      layer, then applies the "global" shader to produce the final image
     *      on the swapchain.
     *      CompositeToSwapchain() 对 game 层应用 "game" 着色器、对 ui 层应用
     *      "ui" 着色器，二者混合进合成层，再对其应用 "global" 着色器输出到交换链。
     *
     * Coordinate system: pixel coordinates with a top-left origin (+Y down).
     * 坐标系：左上角原点、+Y 向下的像素坐标。
     *
     * Typical frame flow (driven by AppRenderer):
     * 典型帧流程（由 AppRenderer 驱动）：
     *   BeginFrame(window)          -> acquire swapchain, clear game/ui layers
     *   DrawTexture/FillRect/...    -> accumulate batches (draw color applies)
     *   EndFrame()                  -> upload batches, draw into game layer
     *   (RmlUi renders into the ui layer on the same command buffer)
     *   CompositeToSwapchain()      -> game/ui/global shader passes
     *   SubmitFrame()               -> submit the command buffer
     */
    class SpriteRenderer {
        /**
         * Single vertex of a sprite quad: position (pixels), uv, color.
         * 精灵四边形的单个顶点：位置（像素）、uv、颜色。
         */
        struct SpriteVertex {
            float x = 0.0F;
            float y = 0.0F;
            float u = 0.0F;
            float v = 0.0F;
            Uint8 r = 255;
            Uint8 g = 255;
            Uint8 b = 255;
            Uint8 a = 255;
        };

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

        std::vector<SpriteVertex> vertices_;
        std::vector<DrawRun> runs_;
        SDL_GPUTexture *currentTexture_ = nullptr;
        SDL_Color drawColor_ = {0, 0, 0, 255};

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
         * @param clear clear 为 true 时用当前 drawColor 清屏，否则保留已有内容
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

        /**
         * Append a textured quad (6 vertices) to the current batch.
         * 向当前批次追加一个带纹理的四边形（6 个顶点）。
         * @param texture texture 采样纹理（nullptr 时使用内置白色纹理）
         * @param positions positions 4 个角点（左上、右上、左下、右下，像素坐标）
         * @param uvs uvs 4 个角点对应的纹理坐标
         * @param color color 顶点颜色（调色/透明度调制）
         */
        void AppendQuad(SDL_GPUTexture *texture, const SDL_FPoint positions[4], const SDL_FPoint uvs[4],
                        const SDL_Color &color);

    public:
        SpriteRenderer() = default;

        ~SpriteRenderer();

        SpriteRenderer(const SpriteRenderer &) = delete;

        SpriteRenderer &operator=(const SpriteRenderer &) = delete;

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
         * and begin the game layer pass with the current draw color.
         * 开始新帧：获取命令缓冲和交换链纹理，必要时重建层纹理，
         * 清除 ui 层并用当前 drawColor 开始 game 层通道。
         * @param window window 目标窗口
         * @return true if a swapchain texture was acquired and drawing is
         * possible, false if the window is minimized/unavailable (drawing
         * calls become no-ops but the other frame functions must still be
         * called).
         * 成功获取交换链纹理返回 true；窗口最小化等情况返回 false
         * （此时绘制调用为空操作，但仍必须调用其他帧函数）。
         */
        bool BeginFrame(SDL_Window *window);

        /**
         * Upload all pending batches, draw them into the game layer and end the
         * game layer render pass.
         * 上传所有待处理批次、绘制进 game 层并结束 game 层渲染通道。
         */
        void EndFrame();

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
         * Set the draw color used by Clear/FillRect/DrawRect/DrawLine/DrawPoint
         * (replaces SDL_SetRenderDrawColor).
         * 设置 Clear/FillRect/DrawRect/DrawLine/DrawPoint 使用的绘制颜色
         * （替代 SDL_SetRenderDrawColor）。
         */
        void SetDrawColor(SDL_Color color);

        /**
         * @return The current draw color (replaces SDL_GetRenderDrawColor).
         * 当前绘制颜色（替代 SDL_GetRenderDrawColor）。
         */
        [[nodiscard]] SDL_Color GetDrawColor() const;

        /**
         * Draw a texture region to a destination rectangle
         * (replaces SDL_RenderTexture).
         * 将纹理区域绘制到目标矩形（替代 SDL_RenderTexture）。
         * @param texture texture 要绘制的纹理
         * @param src src 源矩形（像素，nullptr 表示整个纹理）
         * @param dst dst 目标矩形（像素，nullptr 表示 (0,0,纹理宽,纹理高)）
         * @param mod mod 颜色调制（替代 SDL_SetTextureColorMod/AlphaMod）
         */
        void DrawTexture(const GpuTexture *texture, const SDL_FRect *src, const SDL_FRect *dst,
                         const SDL_Color &mod = {255, 255, 255, 255});

        /**
         * Draw a texture region rotated around a center point, with optional
         * flipping (replaces SDL_RenderTextureRotated).
         * 将纹理区域绕中心点旋转绘制，可选翻转（替代 SDL_RenderTextureRotated）。
         * @param texture texture 要绘制的纹理
         * @param src src 源矩形（像素，nullptr 表示整个纹理）
         * @param dst dst 目标矩形（像素）
         * @param angleDegrees angleDegrees 顺时针角度（度）
         * @param center center 旋转中心（像素，nullptr 表示 dst 中心）
         * @param flip flip FLIP_HORIZONTAL/FLIP_VERTICAL 的按位或
         * @param mod mod 颜色调制
         */
        void DrawTextureRotated(const GpuTexture *texture, const SDL_FRect *src, const SDL_FRect *dst,
                                double angleDegrees, const SDL_FPoint *center, Uint8 flip,
                                const SDL_Color &mod = {255, 255, 255, 255});

        /**
         * Fill a rectangle with the current draw color
         * (replaces SDL_RenderFillRect).
         * 用当前绘制颜色填充矩形（替代 SDL_RenderFillRect）。
         */
        void FillRect(const SDL_FRect *rect);

        /**
         * Draw a 1-pixel rectangle outline with the current draw color
         * (replaces SDL_RenderRect).
         * 用当前绘制颜色绘制 1 像素矩形边框（替代 SDL_RenderRect）。
         */
        void DrawRect(const SDL_FRect *rect);

        /**
         * Draw a 1-pixel line with the current draw color
         * (replaces SDL_RenderLine).
         * 用当前绘制颜色绘制 1 像素线段（替代 SDL_RenderLine）。
         */
        void DrawLine(float x1, float y1, float x2, float y2);

        /**
         * Draw a single pixel with the current draw color
         * (replaces SDL_RenderPoint).
         * 用当前绘制颜色绘制单个像素（替代 SDL_RenderPoint）。
         */
        void DrawPoint(float x, float y);
    };
}
