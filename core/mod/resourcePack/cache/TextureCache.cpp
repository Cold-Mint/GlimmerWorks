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
#include "TextureCache.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3_image/SDL_image.h"


std::shared_ptr<glimmer::TextureResourceResult> glimmer::TextureCache::CreateTexture(const GpuContext *gpuContext,
    const Color &accent, const Color &base) {
    if (gpuContext == nullptr) {
        return nullptr;
    }
    SDL_GPUDevice *gpuDevice = gpuContext->GetDevice();
    if (gpuDevice == nullptr) {
        return nullptr;
    }

    SDL_Surface *surface =
            SDL_CreateSurface(TILE_SIZE, TILE_SIZE, SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr) {
        return nullptr;
    }
    const Uint32 accentValue = SDL_MapSurfaceRGBA(surface, accent.r, accent.g, accent.b, accent.a);
    const Uint32 baseValue = SDL_MapSurfaceRGBA(surface, base.r, base.g, base.b, base.a);
    for (int y = 0; y < TILE_SIZE; ++y) {
        for (int x = 0; x < TILE_SIZE; ++x) {
            const bool isAccentColor =
                    (x < TILE_SIZE / 2 && y < TILE_SIZE / 2) ||
                    (x >= TILE_SIZE / 2 && y >= TILE_SIZE / 2);
            const Uint32 color = isAccentColor ? accentValue : baseValue;
            Uint8 *pixel =
                    static_cast<Uint8 *>(surface->pixels)
                    + y * surface->pitch
                    + x * 4;
            *reinterpret_cast<Uint32 *>(pixel) = color;
        }
    }
    SDL_GPUTexture *texture = CreateTextureFromSurface(gpuDevice, surface);
    SDL_DestroySurface(surface);
    auto textureResourceResult = std::make_unique<TextureResourceResult>();
    textureResourceResult->SetResource(texture);
    textureResourceResult->SetGpuDevice(gpuDevice);
    auto deleter = [](TextureResourceResult *textureResourceResult) {
        if (textureResourceResult == nullptr) {
            return;
        }
        textureResourceResult->DestroyResource();
    };
    return {textureResourceResult.release(), deleter};
}

SDL_GPUTexture *glimmer::TextureCache::CreateTextureFromSurface(SDL_GPUDevice *gpuDevice, SDL_Surface *surface) {
    SDL_Surface *rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ABGR8888);
    if (rgbaSurface == nullptr) {
        return nullptr;
    }
    const int width = rgbaSurface->w;
    const int height = rgbaSurface->h;
    SDL_GPUTextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureCreateInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureCreateInfo.width = static_cast<Uint32>(width);
    textureCreateInfo.height = static_cast<Uint32>(height);
    textureCreateInfo.layer_count_or_depth = 1;
    textureCreateInfo.num_levels = 1;
    textureCreateInfo.sample_count = SDL_GPU_SAMPLECOUNT_1;
    textureCreateInfo.props = 0;
    SDL_GPUTexture *texture = SDL_CreateGPUTexture(gpuDevice, &textureCreateInfo);
    if (texture == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTexture failed: ", SDL_GetError());
        SDL_DestroySurface(rgbaSurface);
        return nullptr;
    }

    const size_t dataSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
    transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferCreateInfo.size = dataSize;
    transferBufferCreateInfo.props = 0;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(gpuDevice, &transferBufferCreateInfo);
    if (transferBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUTransferBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTexture(gpuDevice, texture);
        SDL_DestroySurface(rgbaSurface);
        return nullptr;
    }
    void *mappedPtr = SDL_MapGPUTransferBuffer(gpuDevice, transferBuffer, false);
    if (mappedPtr == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_MapGPUTransferBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
        SDL_ReleaseGPUTexture(gpuDevice, texture);
        SDL_DestroySurface(rgbaSurface);
        return nullptr;
    }
    SDL_LockSurface(rgbaSurface);
    auto *dst = static_cast<Uint8 *>(mappedPtr);
    const auto *src = static_cast<Uint8 *>(rgbaSurface->pixels);
    const int srcPitch = rgbaSurface->pitch;
    const int dstPitch = width * 4;
    for (int y = 0; y < height; ++y) {
        SDL_memcpy(dst + y * dstPitch, src + y * srcPitch, dstPitch);
    }
    SDL_UnlockSurface(rgbaSurface);
    SDL_DestroySurface(rgbaSurface);
    SDL_UnmapGPUTransferBuffer(gpuDevice, transferBuffer);

    SDL_GPUCommandBuffer *uploadCommandBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice);
    if (uploadCommandBuffer == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_AcquireGPUCommandBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
        SDL_ReleaseGPUTexture(gpuDevice, texture);
        return nullptr;
    }
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCommandBuffer);
    if (copyPass == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_BeginGPUCopyPass failed: ", SDL_GetError());
        SDL_CancelGPUCommandBuffer(uploadCommandBuffer);
        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
        SDL_ReleaseGPUTexture(gpuDevice, texture);
        return nullptr;
    }
    SDL_GPUTextureTransferInfo transferInfo = {};
    transferInfo.transfer_buffer = transferBuffer;
    transferInfo.offset = 0;
    transferInfo.pixels_per_row = 0;
    transferInfo.rows_per_layer = 0;
    SDL_GPUTextureRegion textureRegion = {};
    textureRegion.texture = texture;
    textureRegion.mip_level = 0;
    textureRegion.layer = 0;
    textureRegion.x = 0;
    textureRegion.y = 0;
    textureRegion.w = static_cast<Uint32>(width);
    textureRegion.h = static_cast<Uint32>(height);
    textureRegion.d = 1;
    SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(uploadCommandBuffer)) {
        LogCat::w(std::source_location::current(), "SDL_SubmitGPUCommandBuffer failed: ", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
        SDL_ReleaseGPUTexture(gpuDevice, texture);
        return nullptr;
    }
    //The transfer buffer is released right after submission; SDL defers the
    //actual destruction until the GPU has finished the copy.
    //提交后立即释放传输缓冲；SDL 会将实际销毁延迟到 GPU 完成拷贝之后。
    SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
    return texture;
}


glimmer::TextureCache::~TextureCache() noexcept = default;

void glimmer::TextureCache::SetAppContext(const AppContext *appContext) {
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    GpuContext *gpuContext = windowContext->GetGpuContext();
    if (gpuContext == nullptr) {
        return;
    }
    const GraphicsContext *graphicContext = appContext->GetGraphicsContext();
    const PreloadColors *preloadColors = graphicContext->GetPreloadColors();
    if (preloadColors == nullptr) {
        LogCat::w(std::source_location::current(), "preloadColors is nullptr, fallback textures not created");
        return;
    }
    errorTexture_ = CreateTexture(gpuContext, preloadColors->error.accentColor, preloadColors->error.baseColor);
    accessDeniedTexture_ = CreateTexture(gpuContext, preloadColors->accessDenied.accentColor,
                                         preloadColors->accessDenied.baseColor);
}


std::shared_ptr<glimmer::TextureResourceResult> glimmer::TextureCache::CreatePlaceholderResource(
    const AppContext *appContext, const ResourceRef *resourceRef) {
    if (resourceRef->GetPackageId() == RESOURCE_REF_CORE) {
        const std::string resourceKey = resourceRef->GetResourceKey();
        if (resourceKey == ERROR_TEXTURE_KEY) {
            return errorTexture_;
        }
        if (resourceKey == ACCESS_DENIED_TEXTURE_KEY) {
            return accessDeniedTexture_;
        }
    }
    return errorTexture_;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::TextureCache::LoadResourceFromPack(AppContext *appContext,
    const std::filesystem::path &path, const ResourcePack *resourcePack) {
    std::filesystem::path texturePath = resourcePack->GetPath() / "textures" / path;
    texturePath.replace_extension(TEXTURE_FORMAT);
    const VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return nullptr;
    }
    const GpuContext *gpuContext = windowContext->GetGpuContext();
    if (gpuContext == nullptr) {
        return nullptr;
    }
    if (!virtualFileSystem->Exists(texturePath)) {
        return nullptr;
    }
    const auto actualTexturePath = virtualFileSystem->GetActualPath(texturePath);
    if (!actualTexturePath.has_value()) {
        return nullptr;
    }
    SDL_Surface *surface = IMG_Load(actualTexturePath.value().string().c_str());
    if (surface == nullptr) {
        return nullptr;
    }
    SDL_GPUDevice *gpuDevice = gpuContext->GetDevice();
    if (gpuDevice == nullptr) {
        return nullptr;
    }
    SDL_GPUTexture *texture = CreateTextureFromSurface(gpuDevice, surface);
    SDL_DestroySurface(surface);
    auto deleter = [](TextureResourceResult *textureResourceResult) {
        if (textureResourceResult == nullptr) {
            return;
        }
        textureResourceResult->DestroyResource();
        delete textureResourceResult;
    };
    std::shared_ptr<TextureResourceResult> textureResourceResult(new TextureResourceResult(), deleter);
    textureResourceResult->SetResource(texture);
    textureResourceResult->SetGpuDevice(gpuDevice);
    return textureResourceResult;
}
