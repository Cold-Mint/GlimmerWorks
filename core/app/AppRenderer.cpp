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

#include "SDL3_image/SDL_image.h"
#include "core/app/pass/ClearPass.h"
#include "core/app/pass/LightingPass.h"
#include "core/app/pass/ScenePass.h"
#include "core/app/pass/UiPass.h"
#include "core/config/Config.h"
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
#if  !defined(NDEBUG)
    lightingPass_ = dynamic_cast<LightingPass *>(passes_.back().get());
#endif
    passes_.emplace_back(std::make_unique<UiPass>(appContext_->GetRmlContext()));
}

glimmer::AppRenderer::~AppRenderer() {
    if (device_ != nullptr && screenshotTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, screenshotTexture_);
    }
}

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

    const auto pendingScreenshot = appContext_->TakePendingScreenshot();
    SDL_GPUTexture *realSwapChainTexture = swapChainTexture;
    if (pendingScreenshot.has_value()) {
        EnsureScreenshotTexture(logicalWidth, logicalHeight);
        if (screenshotTexture_ != nullptr) {
            ctx.swapChainTexture = screenshotTexture_;
            ctx.swapChainWidth = logicalWidth;
            ctx.swapChainHeight = logicalHeight;
        }
    }

#if  !defined(NDEBUG)
    if (lightingPass_ != nullptr) {
        if (const Config *config = appContext_->GetConfig(); config != nullptr) {
            lightingPass_->SetDisplayLightMap(config->debug.displayLightMap);
            lightingPass_->SetEnableLighting(!config->debug.disableLighting);
        }
    }
#endif
    for (auto &pass: passes_) {
        pass->Prepare(ctx);
    }
    for (auto &pass: passes_) {
        pass->Record(ctx);
    }

    if (pendingScreenshot.has_value()) {
        if (screenshotTexture_ != nullptr && realSwapChainTexture != nullptr) {
            BlitScreenshotToSwapChain(commandBuffer, screenshotTexture_, realSwapChainTexture,
                                      logicalWidth, logicalHeight);
        }
        if (SaveScreenshot(pendingScreenshot.value(), ctx)) {
            return;
        }
        LogCat::w(std::source_location::current(), "screenshot_save_failed",
                  "Failed to save screenshot, falling back to normal frame submission");
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

void glimmer::AppRenderer::EnsureScreenshotTexture(const Uint32 width, const Uint32 height) {
    if (device_ == nullptr || (screenshotTexture_ != nullptr && screenshotTextureWidth_ == width &&
                               screenshotTextureHeight_ == height)) {
        return;
    }
    if (screenshotTexture_ != nullptr) {
        SDL_ReleaseGPUTexture(device_, screenshotTexture_);
        screenshotTexture_ = nullptr;
    }
    SDL_GPUTextureCreateInfo info = {};
    info.type = SDL_GPU_TEXTURETYPE_2D;
    info.format = SDL_GetGPUSwapchainTextureFormat(device_, window_);
    info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    info.width = width;
    info.height = height;
    info.layer_count_or_depth = 1;
    info.num_levels = 1;
    info.sample_count = SDL_GPU_SAMPLECOUNT_1;
    info.props = 0;
    screenshotTexture_ = SDL_CreateGPUTexture(device_, &info);
    if (screenshotTexture_ == nullptr) {
        LogCat::w(std::source_location::current(), "sdl_create_screenshot_texture_failed",
                  "SDL_CreateGPUTexture failed: {}", SDL_GetError());
        return;
    }
    screenshotTextureWidth_ = width;
    screenshotTextureHeight_ = height;
}

void glimmer::AppRenderer::BlitScreenshotToSwapChain(SDL_GPUCommandBuffer *commandBuffer,
                                                     SDL_GPUTexture *source,
                                                     SDL_GPUTexture *destination,
                                                     const Uint32 width, const Uint32 height) {
    if (commandBuffer == nullptr || source == nullptr || destination == nullptr) {
        return;
    }
    SDL_GPUBlitInfo blitInfo = {};
    blitInfo.source.texture = source;
    blitInfo.source.mip_level = 0;
    blitInfo.source.layer_or_depth_plane = 0;
    blitInfo.source.x = 0;
    blitInfo.source.y = 0;
    blitInfo.source.w = width;
    blitInfo.source.h = height;
    blitInfo.destination.texture = destination;
    blitInfo.destination.mip_level = 0;
    blitInfo.destination.layer_or_depth_plane = 0;
    blitInfo.destination.x = 0;
    blitInfo.destination.y = 0;
    blitInfo.destination.w = width;
    blitInfo.destination.h = height;
    blitInfo.load_op = SDL_GPU_LOADOP_DONT_CARE;
    blitInfo.filter = SDL_GPU_FILTER_NEAREST;
    blitInfo.flip_mode = SDL_FLIP_NONE;
    blitInfo.cycle = false;
    SDL_BlitGPUTexture(commandBuffer, &blitInfo);
}

bool glimmer::AppRenderer::SaveScreenshot(const PendingScreenshot &pendingScreenshot,
                                          const RenderFrameContext &ctx) {
    if (pendingScreenshot.onMessage == nullptr || ctx.commandBuffer == nullptr || ctx.swapChainTexture == nullptr) {
        return false;
    }

    SDL_GPUTexture *sourceTexture = ctx.swapChainTexture;
    const auto width = ctx.swapChainWidth;
    const auto height = ctx.swapChainHeight;
    if (width == 0 || height == 0) {
        return false;
    }

    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }

    const auto onMessage = pendingScreenshot.onMessage;
    const auto reportFailure = [&](const std::string_view reason) {
        (*onMessage)(fmt::format(fmt::runtime(langsResources->screenshotSavedFailed), reason));
    };

    const auto pixelDataSize = static_cast<Uint32>(static_cast<size_t>(width) * height * 4);

    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    transferInfo.size = pixelDataSize;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
    if (transferBuffer == nullptr) {
        reportFailure("CreateGPUTransferBuffer failed");
        return false;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(ctx.commandBuffer);
    if (copyPass == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        reportFailure("BeginGPUCopyPass failed");
        return false;
    }

    SDL_GPUTextureRegion source = {};
    source.texture = sourceTexture;
    source.mip_level = 0;
    source.layer = 0;
    source.x = 0;
    source.y = 0;
    source.z = 0;
    source.w = width;
    source.h = height;
    source.d = 1;

    SDL_GPUTextureTransferInfo destination = {};
    destination.transfer_buffer = transferBuffer;
    destination.offset = 0;
    destination.pixels_per_row = width;
    destination.rows_per_layer = height;

    SDL_DownloadFromGPUTexture(copyPass, &source, &destination);
    SDL_EndGPUCopyPass(copyPass);

    SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(ctx.commandBuffer);
    if (fence == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        reportFailure("SubmitGPUCommandBufferAndAcquireFence failed");
        return false;
    }

    SDL_WaitForGPUFences(device_, true, &fence, 1);
    SDL_ReleaseGPUFence(device_, fence);

    void *mapped = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
    if (mapped == nullptr) {
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        reportFailure("MapGPUTransferBuffer failed");
        return false;
    }

    const SDL_PixelFormat pixelFormat = SDL_GetPixelFormatFromGPUTextureFormat(
        SDL_GetGPUSwapchainTextureFormat(device_, window_));
    SDL_Surface *surface = SDL_CreateSurfaceFrom(static_cast<int>(width), static_cast<int>(height),
                                                 pixelFormat, mapped, static_cast<int>(width * 4));
    if (surface == nullptr) {
        SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        reportFailure("CreateSurfaceFrom failed");
        return false;
    }

    if (!IMG_SavePNG(surface, pendingScreenshot.path.string().c_str())) {
        reportFailure(SDL_GetError());
    } else {
        (*onMessage)(fmt::format(fmt::runtime(langsResources->screenshotSavedSuccess),
                                 pendingScreenshot.path.string()));
    }

    SDL_DestroySurface(surface);
    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    return true;
}
