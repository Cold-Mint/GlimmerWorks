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
#include "GpuSamplerCache.h"
#include "core/utils/TomlUtils.h"
#include "toml11/parser.hpp"

std::shared_ptr<glimmer::GPUSamplerResourceResult> glimmer::GpuSamplerCache::LoadResourceFromPack(
    AppContext *appContext, const ResourceRef *resourceRef, const ResourcePack *resourcePack) {
    std::filesystem::path samplerPath = resourcePack->GetPath() / "samplers" / resourceRef->GetPackageId() /
                                        resourceRef->GetResourceKey();
    samplerPath.replace_extension("sampler.toml");
    const VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    if (!virtualFileSystem->Exists(samplerPath)) {
        return nullptr;
    }
    auto data = virtualFileSystem->ReadFileAsString(samplerPath);
    if (!data.has_value()) {
        return nullptr;
    }
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return nullptr;
    }
    SDL_GPUDevice *device = windowContext->GetDevice();
    if (device == nullptr) {
        return nullptr;
    }
    const auto gpuSamplerResource = std::make_unique<GpuSamplerResource>(
        toml::get<GpuSamplerResource>(toml::parse_str(data.value(), TOML_VERSION)));
    if (gpuSamplerResource == nullptr) {
        return nullptr;
    }
    SDL_GPUSamplerCreateInfo createInfo = {};
    createInfo.min_filter = static_cast<SDL_GPUFilter>(gpuSamplerResource->minFilter);
    createInfo.mag_filter = static_cast<SDL_GPUFilter>(gpuSamplerResource->magFilter);
    createInfo.mipmap_mode = static_cast<SDL_GPUSamplerMipmapMode>(gpuSamplerResource->mipmapMode);
    createInfo.address_mode_u = static_cast<SDL_GPUSamplerAddressMode>(gpuSamplerResource->addressModeU);
    createInfo.address_mode_v = static_cast<SDL_GPUSamplerAddressMode>(gpuSamplerResource->addressModeV);
    createInfo.address_mode_w = static_cast<SDL_GPUSamplerAddressMode>(gpuSamplerResource->addressModeW);
    createInfo.mip_lod_bias = gpuSamplerResource->mipLodBias;
    createInfo.max_anisotropy = gpuSamplerResource->maxAnisotropy;
    createInfo.compare_op = static_cast<SDL_GPUCompareOp>(gpuSamplerResource->compareOp);
    createInfo.min_lod = gpuSamplerResource->minLod;
    createInfo.max_lod = gpuSamplerResource->maxLod;
    createInfo.enable_anisotropy = gpuSamplerResource->enableAnisotropy;
    createInfo.enable_compare = gpuSamplerResource->enableCompare;
    SDL_GPUSampler *sampler = SDL_CreateGPUSampler(device, &createInfo);
    if (sampler == nullptr) {
        LogCat::e(std::source_location::current(), "SDL_CreateGPUSampler == nullptr");
        return nullptr;
    }
    auto samplerResourceResult = std::make_shared<GPUSamplerResourceResult>();
    samplerResourceResult->SetDevice(device);
    samplerResourceResult->SetResource(sampler);
    samplerResourceResult->SetResourcePack(resourcePack);
    return samplerResourceResult;
}

glimmer::GpuSamplerCache::~GpuSamplerCache() noexcept = default;
