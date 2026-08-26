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
#include "GpuRenderer.h"

#include <algorithm>

#include "GpuCompositor.h"
#include "GpuFrameState.h"
#include "GpuLayerTextures.h"
#include "GpuLightingPass.h"
#include "GpuRenderPassState.h"
#include "GpuShaderPipelineManager.h"
#include "GpuSpriteBatcher.h"
#include "RenderQueue.h"
#include "core/config/Config.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"

namespace glimmer {
    GpuRenderer::GpuRenderer() = default;

    GpuRenderer::~GpuRenderer() {
        Shutdown();
    }

    bool GpuRenderer::Init(GpuContext *gpuContext, AppContext *appContext) {
        if (gpuContext == nullptr || gpuContext->GetDevice() == nullptr) {
            LogCat::w(std::source_location::current(), "gpuContext is nullptr or not initialized");
            return false;
        }
        if (appContext == nullptr) {
            LogCat::w(std::source_location::current(), "appContext is nullptr");
            return false;
        }
        gpuContext_ = gpuContext;
        frameState_ = std::make_unique<GpuFrameState>();
        renderPassState_ = std::make_unique<GpuRenderPassState>();
        layerTextures_ = std::make_unique<GpuLayerTextures>();
        pipelineManager_ = std::make_unique<GpuShaderPipelineManager>();
        spriteBatcher_ = std::make_unique<GpuSpriteBatcher>();
        lightingPass_ = std::make_unique<GpuLightingPass>();
        compositor_ = std::make_unique<GpuCompositor>();

        const Config *config = appContext->GetConfig();
        const std::filesystem::path cachePath = config != nullptr
                                                    ? std::filesystem::path(config->cachePath)
                                                    : std::filesystem::path(".cache");
        if (!pipelineManager_->Init(gpuContext_->GetDevice(), appContext->GetResourceLocator(),
                                    appContext->GetVirtualFileSystem(), cachePath,
                                    gpuContext_->GetSwapchainFormat())) {
            Shutdown();
            return false;
        }
        if (!spriteBatcher_->Init(gpuContext_)) {
            Shutdown();
            return false;
        }
        if (!compositor_->Init(gpuContext_->GetDevice())) {
            Shutdown();
            return false;
        }
        LogCat::i("GpuRenderer initialized");
        return true;
    }

    void GpuRenderer::Shutdown() {
        if (gpuContext_ == nullptr) {
            return;
        }
        SDL_GPUDevice *device = gpuContext_->GetDevice();
        lightingPass_.reset();
        if (compositor_ != nullptr) {
            compositor_->Shutdown(device);
            compositor_.reset();
        }
        if (spriteBatcher_ != nullptr) {
            spriteBatcher_->Shutdown(device);
            spriteBatcher_.reset();
        }
        if (pipelineManager_ != nullptr) {
            pipelineManager_->Shutdown();
            pipelineManager_.reset();
        }
        if (layerTextures_ != nullptr) {
            layerTextures_->Shutdown();
            layerTextures_.reset();
        }
        if (renderPassState_ != nullptr) {
            renderPassState_->End();
            renderPassState_.reset();
        }
        frameState_.reset();
        gpuContext_ = nullptr;
    }

    bool GpuRenderer::BeginFrame(SDL_Window *window) {
        if (!frameState_->BeginFrame(gpuContext_->GetDevice(), window)) {
            return false;
        }
        if (!layerTextures_->EnsureLayers(gpuContext_, frameState_->GetSwapchainWidth(),
                                          frameState_->GetSwapchainHeight())) {
            frameState_->Invalidate();
            return false;
        }
        //Clear the ui layer to fully transparent; RmlUi later renders into it with LOADOP_LOAD.
        //将 ui 层清除为全透明；RmlUi 随后以 LOADOP_LOAD 渲染进去。
        {
            SDL_GPUColorTargetInfo uiClearTarget = {};
            uiClearTarget.texture = layerTextures_->GetUiLayer()->GetGpuTexture();
            uiClearTarget.mip_level = 0;
            uiClearTarget.layer_or_depth_plane = 0;
            uiClearTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            uiClearTarget.clear_color = {0.0F, 0.0F, 0.0F, 0.0F};
            uiClearTarget.store_op = SDL_GPU_STOREOP_STORE;
            uiClearTarget.cycle = false;
            SDL_GPURenderPass *uiClearPass = SDL_BeginGPURenderPass(frameState_->GetCommandBuffer(), &uiClearTarget, 1,
                                                                    nullptr);
            if (uiClearPass != nullptr) {
                SDL_EndGPURenderPass(uiClearPass);
            }
        }
        renderPassState_->Begin(frameState_->GetCommandBuffer(), layerTextures_->GetGameLayer()->GetGpuTexture(), true,
                                clearColor_);
        return true;
    }

    void GpuRenderer::FlushQueue(RenderQueue &queue) {
        //Consume the light map request even when the frame is inactive so a
        //stale request never leaks into the next frame.
        //即使帧未活跃也消费光照贴图请求，避免过期请求泄漏到下一帧。
        const bool lightingRequested = lightingPass_->ConsumeRequest();
        if (!frameState_->IsActive()) {
            return;
        }
        queue.Sort();
        const std::vector<RenderCommand> &commands = queue.GetCommands();
        if (commands.empty()) {
            renderPassState_->End();
            const bool lightingReady = lightingRequested && lightingPass_->Upload(frameState_->GetCommandBuffer(),
                                           gpuContext_);
            if (lightingReady) {
                renderPassState_->Begin(frameState_->GetCommandBuffer(),
                                        layerTextures_->GetGameLayer()->GetGpuTexture(), false, clearColor_);
                lightingPass_->Draw(frameState_->GetCommandBuffer(), renderPassState_->GetRenderPass(),
                                    pipelineManager_->GetLightingPipeline(), compositor_->GetUnitQuadBuffer(),
                                    gpuContext_->GetLinearSampler());
                renderPassState_->End();
            }
            return;
        }
        //When lighting is active, the queue is split at the RenderLayer::Lighting
        //boundary: world content is drawn first, then the lighting multiply quad,
        //then debug/overlay content (which must stay unshaded).
        //光照激活时，队列在 RenderLayer::Lighting 边界处拆分：先绘制世界内容，
        //然后绘制光照乘法四边形，最后绘制调试/覆盖层内容（它们必须保持无光照）。
        size_t splitIndex = commands.size();
        if (lightingRequested) {
            const auto splitIterator = std::upper_bound(commands.begin(), commands.end(), RenderLayer::Lighting,
                                                        [](const RenderLayer layer, const RenderCommand &command) {
                                                            return layer < command.layer;
                                                        });
            splitIndex = static_cast<size_t>(std::distance(commands.begin(), splitIterator));
        }
        GpuSpriteBatcher::Batch batch = spriteBatcher_->Build(commands, splitIndex);
        renderPassState_->End();
        //Upload the light map while no render pass is active (copy passes cannot
        //be recorded inside a render pass).
        //在没有活动渲染通道时上传光照贴图（拷贝通道无法在渲染通道内记录）。
        const bool lightingReady = lightingRequested && lightingPass_->Upload(frameState_->GetCommandBuffer(),
                                                                              gpuContext_);
        if (batch.vertices.empty()) {
            if (lightingReady) {
                renderPassState_->Begin(frameState_->GetCommandBuffer(),
                                        layerTextures_->GetGameLayer()->GetGpuTexture(), false, clearColor_);
                lightingPass_->Draw(frameState_->GetCommandBuffer(), renderPassState_->GetRenderPass(),
                                    pipelineManager_->GetLightingPipeline(), compositor_->GetUnitQuadBuffer(),
                                    gpuContext_->GetLinearSampler());
                renderPassState_->End();
            }
            return;
        }
        if (!spriteBatcher_->Upload(gpuContext_->GetDevice(), frameState_->GetCommandBuffer(), batch.vertices)) {
            return;
        }
        renderPassState_->Begin(frameState_->GetCommandBuffer(), layerTextures_->GetGameLayer()->GetGpuTexture(), false,
                                clearColor_);
        SDL_GPURenderPass *renderPass = renderPassState_->GetRenderPass();
        SDL_GPUGraphicsPipeline *spritePipeline = pipelineManager_->GetSpritePipeline();
        if (renderPass == nullptr || spritePipeline == nullptr) {
            renderPassState_->End();
            return;
        }
        const Uint32 swapchainWidth = frameState_->GetSwapchainWidth();
        const Uint32 swapchainHeight = frameState_->GetSwapchainHeight();
        SDL_GPUViewport viewport = {};
        viewport.x = 0.0F;
        viewport.y = 0.0F;
        viewport.w = static_cast<float>(swapchainWidth);
        viewport.h = static_cast<float>(swapchainHeight);
        viewport.min_depth = 0.0F;
        viewport.max_depth = 1.0F;
        SDL_SetGPUViewport(renderPass, &viewport);
        const float viewSize[2] = {
            static_cast<float>(swapchainWidth),
            static_cast<float>(swapchainHeight)
        };
        SDL_BindGPUGraphicsPipeline(renderPass, spritePipeline);
        SDL_PushGPUVertexUniformData(frameState_->GetCommandBuffer(), 0, viewSize, sizeof(viewSize));
        SDL_GPUBufferBinding vertexBufferBinding = {};
        vertexBufferBinding.buffer = spriteBatcher_->GetVertexBuffer();
        vertexBufferBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBufferBinding, 1);
        GpuSpriteBatcher::DrawRuns(renderPass, batch.worldRuns, gpuContext_->GetNearestSampler());
        if (lightingReady) {
            lightingPass_->Draw(frameState_->GetCommandBuffer(), renderPass, pipelineManager_->GetLightingPipeline(),
                                compositor_->GetUnitQuadBuffer(), gpuContext_->GetLinearSampler());
            if (!batch.overlayRuns.empty()) {
                //Restore the sprite pipeline state for the overlay batch.
                //为覆盖层批次恢复精灵管线状态。
                SDL_BindGPUGraphicsPipeline(renderPass, spritePipeline);
                SDL_PushGPUVertexUniformData(frameState_->GetCommandBuffer(), 0, viewSize, sizeof(viewSize));
                SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBufferBinding, 1);
                GpuSpriteBatcher::DrawRuns(renderPass, batch.overlayRuns, gpuContext_->GetNearestSampler());
            }
        } else {
            GpuSpriteBatcher::DrawRuns(renderPass, batch.overlayRuns, gpuContext_->GetNearestSampler());
        }
        renderPassState_->End();
    }

    void GpuRenderer::SetLightMap(const int width, const int height, const Uint8 *rgbaPixels,
                                  const LightMapParams &params) {
        lightingPass_->SetLightMap(width, height, rgbaPixels, params);
    }

    GpuTexture *GpuRenderer::GetUiTargetTexture() const {
        return layerTextures_->GetUiLayer();
    }

    void GpuRenderer::CompositeToSwapchain() {
        if (!frameState_->IsActive()) {
            return;
        }
        compositor_->Composite(frameState_->GetCommandBuffer(), *renderPassState_, *pipelineManager_, *layerTextures_,
                               frameState_->GetSwapchainTexture(),
                               frameState_->GetSwapchainWidth(), frameState_->GetSwapchainHeight(),
                               gpuContext_->GetNearestSampler());
    }

    bool GpuRenderer::SubmitFrame() {
        return frameState_->SubmitFrame();
    }

    SDL_GPUCommandBuffer *GpuRenderer::GetCommandBuffer() const {
        return frameState_->GetCommandBuffer();
    }

    SDL_GPUTexture *GpuRenderer::GetSwapchainTexture() const {
        return frameState_->GetSwapchainTexture();
    }

    Uint32 GpuRenderer::GetSwapchainWidth() const {
        return frameState_->GetSwapchainWidth();
    }

    Uint32 GpuRenderer::GetSwapchainHeight() const {
        return frameState_->GetSwapchainHeight();
    }

    void GpuRenderer::SetClearColor(const SDL_Color color) {
        clearColor_ = color;
    }

    SDL_Color GpuRenderer::GetClearColor() const {
        return clearColor_;
    }
}
