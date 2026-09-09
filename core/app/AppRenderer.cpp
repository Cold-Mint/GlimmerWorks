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
#include "core/app/AppRenderer.h"

#include <memory>

#include "core/app/pass/ClearPass.h"
#include "core/app/pass/LightingPass.h"
#include "core/app/pass/ScenePass.h"
#include "core/app/pass/UiPass.h"
#include "core/config/Constants.h"
#include "core/gpu/IPass.h"
#include "core/context/WindowContext.h"
#include "core/gpu/RenderFrameContext.h"
#include "core/gpu/UniformInjectContext.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/GPUPipelineResourceResult.h"
#include "core/scene/Scene.h"
#include "core/scene/SceneManager.h"


glimmer::AppRenderer::AppRenderer(AppContext *appContext) : appContext_(appContext) {
    if (appContext_ == nullptr) {
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext != nullptr) {
        device_ = windowContext->GetDevice();
        window_ = windowContext->GetWindow();
    }
    resourceLocator_ = appContext_->GetResourceLocator();
    ResourceRef defaultPipelineResourceRef;
    defaultPipelineResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    defaultPipelineResourceRef.SetResourceType(RESOURCE_PIPELINE);
    defaultPipelineResourceRef.SetResourceKey("default");
    auto defaultPipeline = resourceLocator_->FindGPUGraphicsPipeline(&defaultPipelineResourceRef);
    if (defaultPipeline == nullptr) {
        LogCat::e(std::source_location::current(), "default_pipeline_failed", "defaultPipeline failed: {}",
                  SDL_GetError());
    }
    ResourceRef defaultSamplerResourceRef;
    defaultSamplerResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    defaultSamplerResourceRef.SetResourceType(RESOURCE_SAMPLER);
    defaultSamplerResourceRef.SetResourceKey("default");
    auto defaultSampler = resourceLocator_->FindGPUGraphicsSampler(&defaultSamplerResourceRef);
    if (defaultSampler == nullptr) {
        LogCat::e(std::source_location::current(), "default_sampler_failed", "defaultSampler failed: {}",
                  SDL_GetError());
    }
    ResourceRef lightingPipelineResourceRef;
    lightingPipelineResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightingPipelineResourceRef.SetResourceType(RESOURCE_PIPELINE);
    lightingPipelineResourceRef.SetResourceKey("lighting");
    auto lightingPipeline = resourceLocator_->FindGPUGraphicsPipeline(&lightingPipelineResourceRef);
    ResourceRef lightingSamplerResourceRef;
    lightingSamplerResourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    lightingSamplerResourceRef.SetResourceType(RESOURCE_SAMPLER);
    lightingSamplerResourceRef.SetResourceKey("lighting");
    auto lightingSampler = resourceLocator_->FindGPUGraphicsSampler(&lightingSamplerResourceRef);
    sceneManager_ = appContext_->GetSceneManager();
    passes_.emplace_back(
        std::make_unique<ScenePass>(device_, window_, std::move(defaultPipeline), std::move(defaultSampler)));
    passes_.emplace_back(std::make_unique<ClearPass>());
    passes_.emplace_back(std::make_unique<LightingPass>(resourceLocator_, device_, std::move(lightingPipeline),
                                                        std::move(lightingSampler)));
    passes_.emplace_back(std::make_unique<UiPass>(appContext_->GetRmlContext()));
}

glimmer::AppRenderer::~AppRenderer() = default;

void glimmer::AppRenderer::RenderFrame(const int windowWidth, const int windowHeight) {
    if (windowWidth <= 0 || windowHeight <= 0 || device_ == nullptr || sceneManager_ == nullptr) {
        return;
    }
    Scene *topScene = sceneManager_->GetTopScene();
    if (topScene == nullptr) {
        return;
    }
    renderQueue_.Clear();
    topScene->Render(&renderQueue_);
    RenderOverlays();
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer == nullptr) {
        return;
    }
    SDL_GPUTexture *swapChainTexture = nullptr;
    Uint32 swapChainWidth = 0;
    Uint32 swapChainHeight = 0;
    if (!SDL_AcquireGPUSwapchainTexture(commandBuffer, window_, &swapChainTexture, &swapChainWidth, &swapChainHeight)) {
        LogCat::w(std::source_location::current(), "sdl_acquire_gpu_swapchain_texture_failed",
                  "SDL_AcquireGPUSwapChainTexture failed: {}", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer);
        return;
    }

    const auto logicalWidth = static_cast<Uint32>(windowWidth);
    const auto logicalHeight = static_cast<Uint32>(windowHeight);
    UniformInjectContext *uniformInjectContext = topScene->GetUniformInjectContext();
    if (uniformInjectContext != nullptr) {
        uniformInjectContext->width = static_cast<float>(logicalWidth);
        uniformInjectContext->height = static_cast<float>(logicalHeight);
    } else {
        LogCat::w(std::source_location::current(), "app_renderer_uniform_inject_context_is_null",
                  "UniformInjectContext is null, lighting pass will be skipped (black screen)");
    }

    RenderFrameContext ctx;
    ctx.commandBuffer = commandBuffer;
    ctx.swapChainTexture = swapChainTexture;
    ctx.swapChainWidth = swapChainWidth;
    ctx.swapChainHeight = swapChainHeight;
    ctx.logicalWidth = logicalWidth;
    ctx.logicalHeight = logicalHeight;
    ctx.injectContext = uniformInjectContext;
    ctx.renderQueue = &renderQueue_;
    for (auto &pass: passes_) {
        pass->Prepare(ctx);
    }
    for (auto &pass: passes_) {
        pass->Record(ctx);
    }
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        LogCat::w(std::source_location::current(), "sdl_submit_gpu_command_buffer_failed",
                  "SDL_SubmitGPUCommandBuffer failed: {}", SDL_GetError());
    }
}

void glimmer::AppRenderer::RenderOverlays() {
    SceneManager *sceneManager = appContext_->GetSceneManager();
    if (sceneManager == nullptr) {
        return;
    }
    const auto &overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        overlay->Render(&renderQueue_);
    }
}
