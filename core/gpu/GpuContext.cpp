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
#include "GpuContext.h"

#include "core/log/LogCat.h"

glimmer::GpuContext::~GpuContext() {
    Shutdown();
}

bool glimmer::GpuContext::Init(SDL_Window *window, const bool vSync) {
    if (window == nullptr) {
        LogCat::w(std::source_location::current(), "window is nullptr");
        return false;
    }
    device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (device_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUDevice failed: ", SDL_GetError());
        return false;
    }
    LogCat::i("GPU device created, driver: ", SDL_GetGPUDeviceDriver(device_));
    if (!SDL_ClaimWindowForGPUDevice(device_, window)) {
        LogCat::e(std::source_location::current(), "SDL_ClaimWindowForGPUDevice failed: ", SDL_GetError());
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
        return false;
    }
    window_ = window;
    if (!SDL_SetGPUSwapchainParameters(device_, window_, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                       vSync ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE)) {
        LogCat::w(std::source_location::current(), "SDL_SetGPUSwapchainParameters failed: ", SDL_GetError());
    }

    SDL_GPUSamplerCreateInfo samplerInfo = {};
    samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.mip_lod_bias = 0.0F;
    samplerInfo.max_anisotropy = 1.0F;
    samplerInfo.min_lod = 0.0F;
    samplerInfo.max_lod = 0.0F;
    nearestSampler_ = SDL_CreateGPUSampler(device_, &samplerInfo);
    if (nearestSampler_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUSampler(nearest) failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    linearSampler_ = SDL_CreateGPUSampler(device_, &samplerInfo);
    if (linearSampler_ == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUSampler(linear) failed: ", SDL_GetError());
        Shutdown();
        return false;
    }
    LogCat::i("GpuContext initialized");
    return true;
}

void glimmer::GpuContext::Shutdown() {
    if (device_ != nullptr) {
        if (linearSampler_ != nullptr) {
            SDL_ReleaseGPUSampler(device_, linearSampler_);
            linearSampler_ = nullptr;
        }
        if (nearestSampler_ != nullptr) {
            SDL_ReleaseGPUSampler(device_, nearestSampler_);
            nearestSampler_ = nullptr;
        }
        if (window_ != nullptr) {
            SDL_ReleaseWindowFromGPUDevice(device_, window_);
            window_ = nullptr;
        }
        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;
    }
}

SDL_GPUDevice *glimmer::GpuContext::GetDevice() const {
    return device_;
}

SDL_Window *glimmer::GpuContext::GetWindow() const {
    return window_;
}

SDL_GPUTextureFormat glimmer::GpuContext::GetSwapchainFormat() const {
    if (device_ == nullptr || window_ == nullptr) {
        return SDL_GPU_TEXTUREFORMAT_INVALID;
    }
    return SDL_GetGPUSwapchainTextureFormat(device_, window_);
}

SDL_GPUSampler *glimmer::GpuContext::GetNearestSampler() const {
    return nearestSampler_;
}

SDL_GPUSampler *glimmer::GpuContext::GetLinearSampler() const {
    return linearSampler_;
}

glimmer::GpuTexture *glimmer::GpuContext::CreateTargetTexture(const Uint32 width, const Uint32 height,
                                                              const SDL_GPUTextureFormat format) const {
    if (device_ == nullptr || width == 0 || height == 0 || format == SDL_GPU_TEXTUREFORMAT_INVALID) {
        LogCat::w(std::source_location::current(), "invalid CreateTargetTexture arguments");
        return nullptr;
    }
    SDL_GPUTextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureCreateInfo.format = format;
    textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureCreateInfo.width = width;
    textureCreateInfo.height = height;
    textureCreateInfo.layer_count_or_depth = 1;
    textureCreateInfo.num_levels = 1;
    textureCreateInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureCreateInfo.props = 0;
    SDL_GPUTexture *texture = SDL_CreateGPUTexture(device_, &textureCreateInfo);
    if (texture == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture(target) failed: ", SDL_GetError());
        return nullptr;
    }
    return new GpuTexture(device_, texture, static_cast<int>(width), static_cast<int>(height));
}

glimmer::GpuTexture *glimmer::GpuContext::CreateSampledTexture(const Uint32 width, const Uint32 height,
                                                               const SDL_GPUTextureFormat format) const {
    if (device_ == nullptr || width == 0 || height == 0 || format == SDL_GPU_TEXTUREFORMAT_INVALID) {
        LogCat::w(std::source_location::current(), "invalid CreateSampledTexture arguments");
        return nullptr;
    }
    SDL_GPUTextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureCreateInfo.format = format;
    textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureCreateInfo.width = width;
    textureCreateInfo.height = height;
    textureCreateInfo.layer_count_or_depth = 1;
    textureCreateInfo.num_levels = 1;
    textureCreateInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureCreateInfo.props = 0;
    SDL_GPUTexture *texture = SDL_CreateGPUTexture(device_, &textureCreateInfo);
    if (texture == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture(sampled) failed: ", SDL_GetError());
        return nullptr;
    }
    return new GpuTexture(device_, texture, static_cast<int>(width), static_cast<int>(height));
}

SDL_Surface *glimmer::GpuContext::ReadbackTexture(SDL_GPUTexture *texture, const Uint32 width,
                                                  const Uint32 height, const SDL_PixelFormat pixelFormat) const {
    if (device_ == nullptr || texture == nullptr || width == 0 || height == 0 ||
        pixelFormat == SDL_PIXELFORMAT_UNKNOWN) {
        LogCat::w(std::source_location::current(), "invalid readback arguments");
        return nullptr;
    }
    const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
    transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    transferBufferCreateInfo.size = dataSize;
    transferBufferCreateInfo.props = 0;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferBufferCreateInfo);
    if (transferBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
        return nullptr;
    }
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (commandBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUCommandBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (copyPass == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_BeginGPUCopyPass failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(commandBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    SDL_GPUTextureRegion textureRegion = {};
    textureRegion.texture = texture;
    textureRegion.mip_level = 0;
    textureRegion.layer = 0;
    textureRegion.x = 0;
    textureRegion.y = 0;
    textureRegion.w = width;
    textureRegion.h = height;
    textureRegion.d = 1;
    SDL_GPUTextureTransferInfo transferInfo = {};
    transferInfo.transfer_buffer = transferBuffer;
    transferInfo.offset = 0;
    transferInfo.pixels_per_row = width;
    transferInfo.rows_per_layer = height;
    SDL_DownloadFromGPUTexture(copyPass, &textureRegion, &transferInfo);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    if (!SDL_WaitForGPUIdle(device_)) {
        LogCat::w(std::source_location::current(), "SDL_WaitForGPUIdle failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    void *mappedPtr = SDL_MapGPUTransferBuffer(device_, transferBuffer, false);
    if (mappedPtr == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    SDL_Surface *surface = SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height),
                                             pixelFormat);
    if (surface == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateSurface failed: ", SDL_GetError());
        SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
        SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
        return nullptr;
    }
    SDL_LockSurface(surface);
    auto *dst = static_cast<Uint8 *>(surface->pixels);
    const auto *src = static_cast<Uint8 *>(mappedPtr);
    const int dstPitch = surface->pitch;
    const int srcPitch = static_cast<int>(width) * 4;
    for (Uint32 y = 0; y < height; ++y) {
        SDL_memcpy(dst + y * dstPitch, src + y * srcPitch, srcPitch);
    }
    SDL_UnlockSurface(surface);
    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);
    return surface;
}
