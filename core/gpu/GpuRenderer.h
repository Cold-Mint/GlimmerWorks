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
#include <string>
#include <unordered_map>
#include <vector>

#include "DrawRun.h"
#include "GpuContext.h"
#include "GpuShaderCache.h"
#include "GpuTexture.h"
#include "LightMapParams.h"
#include "RenderCommand.h"
#include "SDL3/SDL_pixels.h"

namespace glimmer {
    class RenderQueue;
    class ResourceLocator;
    class VirtualFileSystem;
    class AppContext;

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
     * Pipelines are compiled lazily on first use (never at startup), reusing
     * disk-cached SPIR-V binaries when the shader source did not change.
     * 基于 SDL_GPU 的帧级渲染器。它持有图形管线、动态顶点缓冲和离屏层纹理，
     * 并绘制 RenderQueue 的内容（见 FlushQueue）。它替代了旧的立即模式
     * SpriteRenderer：游戏系统不再在每次绘制调用时累积顶点，而是把
     * RenderCommand 记录进 RenderQueue，由渲染器每帧一次性消费整个
     * 排好序的队列。管线在首次使用时惰性编译（启动阶段不预编译），
     * 着色器源码未变化时复用磁盘缓存的 SPIR-V 二进制。
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
         * Blend configuration of a sprite-style pipeline.
         * 精灵风格管线的混合配置。
         */
        enum class SpriteBlendMode : uint8_t {
            /**
             * Standard alpha blending (src.a / 1-src.a).
             * 标准 alpha 混合（src.a / 1-src.a）。
             */
            Alpha,
            /**
             * Multiplicative blending (dst.rgb * src.rgb), used by the
             * lighting pass to modulate the game layer.
             * 乘法混合（dst.rgb * src.rgb），光照通道用它调制 game 层。
             */
            Multiply,
            /**
             * Blending disabled (source overwrites destination).
             * 禁用混合（源直接覆盖目标）。
             */
            None
        };

        static constexpr Uint32 INITIAL_VERTEX_CAPACITY = 65536;

        GpuContext *gpuContext_ = nullptr;
        SDL_GPUDevice *device_ = nullptr;
        ResourceLocator *resourceLocator_ = nullptr;
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        /**
         * Disk cache of compiled SPIR-V binaries (keyed by shader resource
         * reference, validated via mtime/blake3). Created in Init; pipelines
         * and shaders are compiled lazily on first use, never at startup.
         * 编译后 SPIR-V 二进制的磁盘缓存（按着色器资源引用索引，通过
         * 修改时间/blake3 校验）。在 Init 中创建；管线与着色器在首次使用时
         * 惰性编译，启动阶段不预编译。
         */
        std::unique_ptr<GpuShaderCache> gpuShaderCache_ = nullptr;
        /**
         * In-memory cache of loaded shaders (key: "<name>.<ext>", e.g.
         * "sprite.vert") and created pipelines (key: pipeline name, e.g.
         * "sprite"/"game"/"ui"/"global"/"lighting"). Entries are created on
         * first use and released in Shutdown.
         * 已加载着色器的内存缓存（键："<名称>.<扩展名>"，如 "sprite.vert"）与
         * 已创建管线的内存缓存（键：管线名，如
         * "sprite"/"game"/"ui"/"global"/"lighting"）。条目在首次使用时创建，
         * 在 Shutdown 中释放。
         */
        std::unordered_map<std::string, SDL_GPUShader *> shaderCache_;
        std::unordered_map<std::string, SDL_GPUGraphicsPipeline *> pipelineCache_;
        std::unique_ptr<GpuTexture> whiteTexture_ = nullptr;

        std::unique_ptr<GpuTexture> gameLayerTexture_ = nullptr;
        std::unique_ptr<GpuTexture> uiLayerTexture_ = nullptr;
        std::unique_ptr<GpuTexture> compositeLayerTexture_ = nullptr;

        /**
         * Per-tile light map texture (1 texel = 1 tile) sampled by the
         * lighting pass, plus its CPU-side staging copy and shader params.
         * 光照通道采样的逐瓦片光照贴图纹理（1 texel = 1 瓦片），
         * 以及对应的 CPU 暂存副本与着色器参数。
         */
        std::unique_ptr<GpuTexture> lightMapTexture_ = nullptr;
        std::vector<Uint8> lightMapPixels_;
        int lightMapWidth_ = 0;
        int lightMapHeight_ = 0;
        LightMapParams lightMapParams_{};
        /**
         * lightMapPending_: a lighting system provided light map data this
         * frame (consumed by FlushQueue). lightMapDirty_: the staging pixels
         * changed and must be re-uploaded to the GPU texture.
         * lightMapPending_：光照系统本帧提供了光照贴图数据（由 FlushQueue 消费）。
         * lightMapDirty_：暂存像素已变化，需要重新上传到 GPU 纹理。
         */
        bool lightMapPending_ = false;
        bool lightMapDirty_ = false;

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
         * @param blendMode blendMode 混合模式（alpha / 乘法 / 禁用）
         * @return The pipeline on success, nullptr on failure.
         * 成功返回管线，失败返回 nullptr。
         */
        SDL_GPUGraphicsPipeline *CreateSpritePipeline(SDL_GPUShader *vertexShader, SDL_GPUShader *fragmentShader,
                                                       SpriteBlendMode blendMode) const;

        /**
         * Get a shader from the in-memory cache, loading it on first use:
         * the GLSL source is located through the ResourceLocator
         * (RESOURCE_SHADER), a valid disk-cached SPIR-V binary is used when
         * available, otherwise the source is compiled and the result is
         * written to the disk cache. Falls back to the embedded pass-through
         * source when the resource packs do not provide the shader or
         * compilation fails.
         * 从内存缓存获取着色器，首次使用时加载：GLSL 源码通过资源定位器
         * （RESOURCE_SHADER）查找，存在有效的磁盘缓存 SPIR-V 时直接使用，
         * 否则编译源码并把结果写入磁盘缓存。材质包未提供该着色器或编译失败时
         * 回退到内嵌的 pass-through 源码。
         * @param key key 着色器名（如 sprite、game、lighting）
         * @param extension extension 扩展名（"vert" 或 "frag"）
         * @param stage stage 着色器阶段
         * @param numSamplers numSamplers 采样器数量
         * @param numUniformBuffers numUniformBuffers uniform 缓冲数量
         * @param fallbackSource fallbackSource 内嵌兜底 GLSL 源码
         * @return The shader (owned by shaderCache_), nullptr on failure.
         * 着色器（由 shaderCache_ 持有），失败返回 nullptr。
         */
        SDL_GPUShader *GetOrCreateShader(const std::string &key, const std::string &extension,
                                         SDL_GPUShaderStage stage, Uint32 numSamplers, Uint32 numUniformBuffers,
                                         const char *fallbackSource);

        /**
         * Get a pipeline from the in-memory cache, creating it (and its
         * shaders) lazily on first use. The fragment shader shares the
         * pipeline name (e.g. pipeline "game" uses shaders/@core/game.frag).
         * 从内存缓存获取管线，首次使用时惰性创建管线（及其着色器）。片元着色器
         * 与管线同名（如管线 "game" 使用 shaders/@core/game.frag）。
         * @param name name 管线名（sprite/game/ui/global/lighting）
         * @param blendMode blendMode 混合模式
         * @param numSamplers numSamplers 片元着色器采样器数量
         * @param numUniformBuffers numUniformBuffers 片元着色器 uniform 缓冲数量
         * @param fallbackFragSource fallbackFragSource 内嵌兜底片元着色器源码
         * @return The pipeline (owned by pipelineCache_), nullptr on failure.
         * 管线（由 pipelineCache_ 持有），失败返回 nullptr。
         */
        SDL_GPUGraphicsPipeline *GetOrCreatePipeline(const std::string &name, SpriteBlendMode blendMode,
                                                     Uint32 numSamplers, Uint32 numUniformBuffers,
                                                     const char *fallbackFragSource);

        /**
         * Lazy accessors for the five built-in pipelines. They compile the
         * required shaders (with disk-cache reuse) and create the pipeline on
         * first call; subsequent calls return the cached pointer without any
         * disk IO.
         * 五条内置管线的惰性访问器。首次调用时编译所需着色器（复用磁盘缓存）
         * 并创建管线；后续调用直接返回缓存指针，不再进行磁盘 IO。
         */
        SDL_GPUGraphicsPipeline *GetSpritePipeline();
        SDL_GPUGraphicsPipeline *GetGamePipeline();
        SDL_GPUGraphicsPipeline *GetUiPipeline();
        SDL_GPUGraphicsPipeline *GetGlobalPipeline();
        SDL_GPUGraphicsPipeline *GetLightingPipeline();

        /**
         * Expand a range of sorted render commands into batch vertices and
         * texture-grouped draw runs (appended to the output vectors, so two
         * ranges can share one vertex buffer upload).
         * 把一段已排序的渲染命令展开为批处理顶点与按纹理分组的绘制段
         * （追加到输出向量，因此两段范围可以共享一次顶点缓冲上传）。
         * @param commands commands 命令数组起始地址
         * @param count count 命令数量
         * @param vertices vertices 顶点输出向量（追加）
         * @param runs runs 绘制段输出向量（追加）
         */
        void ExpandCommands(const RenderCommand *commands, size_t count, std::vector<SpriteVertex> &vertices,
                            std::vector<DrawRun> &runs) const;

        /**
         * Create/recreate the light map texture when its size changed and
         * upload the staging pixels through a copy pass on the frame command
         * buffer. Must be called while no render pass is active.
         * 当光照贴图尺寸变化时创建/重建纹理，并通过帧命令缓冲上的拷贝通道
         * 上传暂存像素。必须在没有活动渲染通道时调用。
         * @return true if the GPU light map is valid and up to date.
         * GPU 光照贴图有效且为最新时返回 true。
         */
        bool EnsureLightMapUploaded();

        /**
         * Draw the full-screen lighting quad: samples the light map with the
         * linear sampler and multiplies everything drawn so far by the
         * computed light multiplier (Multiply blend pipeline). Must be
         * called inside an active render pass on the game layer.
         * 绘制全屏光照四边形：用线性采样器采样光照贴图，并把计算出的光照系数
         * 乘法混合到已绘制内容上（乘法混合管线）。必须在 game 层的活动渲染通道内调用。
         */
        void DrawLightingQuad();

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
         * Initialize the renderer: create the dynamic vertex buffer, the
         * built-in white texture and the full-screen unit quad buffer, and
         * prepare the shader disk cache. No shader is compiled and no
         * graphics pipeline is created here — that happens lazily on first
         * use (see GetSpritePipeline and friends).
         * 初始化渲染器：创建动态顶点缓冲、内置白色纹理、全屏单位四边形缓冲，
         * 并准备好着色器磁盘缓存。此处不编译任何着色器、不创建任何图形管线——
         * 这些工作在首次使用时惰性进行（见 GetSpritePipeline 等）。
         * @param gpuContext gpuContext 已初始化的 GPU 上下文
         * @param appContext appContext 应用上下文（提供资源定位器、虚拟文件系统与缓存路径配置）
         * @return true on success, false on failure (error is logged).
         * 成功返回 true，失败返回 false（错误会记录日志）。
         */
        bool Init(GpuContext *gpuContext, AppContext *appContext);

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
         * When a lighting system provided a light map this frame (see
         * SetLightMap), the queue is split at the RenderLayer::Lighting
         * boundary and a full-screen multiply-blended lighting quad is drawn
         * between the two batches, so world content (Background..Lighting)
         * is shaded while debug/overlay content stays untouched.
         * 对渲染队列排序（层升序，然后 depth 升序），把每个命令展开为批处理
         * 顶点，上传并绘制进 game 层，然后结束 game 层渲染通道。
         * 当光照系统在本帧提供了光照贴图（见 SetLightMap）时，队列会在
         * RenderLayer::Lighting 边界处拆分为两批，并在两批之间绘制一个
         * 全屏乘法混合的光照四边形，使世界内容（Background..Lighting）
         * 被着色，而调试/覆盖层内容不受影响。
         * @param queue queue 本帧填充完毕的渲染队列
         */
        void FlushQueue(RenderQueue &queue);

        /**
         * Provide this frame's light map (called by the lighting system after
         * BeginFrame and before FlushQueue). The params are consumed every
         * frame; the pixels are only re-uploaded when they change (pass
         * nullptr to reuse the previous GPU texture contents with new params).
         * 提供本帧的光照贴图（由光照系统在 BeginFrame 之后、FlushQueue 之前调用）。
         * 参数每帧都会生效；像素仅在变化时重新上传（传 nullptr 可沿用
         * GPU 纹理中已有的内容，仅更新参数）。
         * @param width width 光照贴图宽度（texel = 瓦片）
         * @param height height 光照贴图高度（texel = 瓦片）
         * @param rgbaPixels rgbaPixels RGBA8 像素（width*height*4 字节），nullptr 表示复用
         * @param params params 光照片元着色器的 uniform 参数
         */
        void SetLightMap(int width, int height, const Uint8 *rgbaPixels, const LightMapParams &params);

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
