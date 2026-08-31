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
#include "ShaderCache.h"

#include <src/cache/shader_cache.pb.h>

#include "core/gpu/GpuShaderCompiler.h"
#include "core/mod/resourcePack/ShaderResourceResult.h"
#include "core/utils/StringUtils.h"


std::shared_ptr<glimmer::ShaderResourceResult> glimmer::ShaderCache::LoadResourceFromPack(AppContext *appContext,
    const ResourceRef *resourceRef, const ResourcePack *resourcePack) {
    WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return nullptr;
    }
    SDL_GPUDevice *device = windowContext->GetDevice();
    if (device == nullptr) {
        return nullptr;
    }
    std::filesystem::path shaderPath = resourcePack->GetPath() / "shaders" / resourceRef->GetPackageId() / resourceRef->
                                       GetResourceKey();
    const auto sourceMtime = virtualFileSystem_->GetMtime(shaderPath);
    if (!sourceMtime.has_value()) {
        //The source file modification time cannot be obtained.
        //无法获取源文件修改时间。
        return nullptr;
    }
    auto mtimeInt64 = VirtualFileSystem::FileTimeTypeToInt64(sourceMtime.value());
    bool vertex = resourceRef->GetResourceType() == RESOURCE_SHADER_VERTEX;
    if (vertex) {
        shaderPath.replace_extension(SHADER_VERT_FORMAT);
    } else {
        shaderPath.replace_extension(SHADER_FRAG_FORMAT);
    }
    const VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    if (!virtualFileSystem->Exists(shaderPath)) {
        return nullptr;
    }
    SDL_GPUShaderCreateInfo shaderInfo = {};
    shaderInfo.entrypoint = SHADER_ENTRY_POINT.c_str();
    shaderInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage = vertex ? SDL_GPU_SHADERSTAGE_VERTEX : SDL_GPU_SHADERSTAGE_FRAGMENT;
    shaderInfo.num_storage_textures = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.props = 0;
    auto shaderCacheMessagePtr = TryLoad(mtimeInt64, resourceRef, shaderPath);
    if (shaderCacheMessagePtr == nullptr) {
        auto dataOptional = ReadSource(shaderPath);
        if (!dataOptional.has_value()) {
            return nullptr;
        }
        std::unique_ptr<GpuShaderCompileResult> gpuShaderCompileResult = GpuShaderCompiler::CompileToSpirv(
            dataOptional.value(), vertex);
        if (gpuShaderCompileResult == nullptr) {
            return nullptr;
        }
        shaderInfo.code = reinterpret_cast<const uint8_t *>(gpuShaderCompileResult->GetCode().data());
        shaderInfo.code_size = gpuShaderCompileResult->GetCodeSize();
        shaderInfo.num_samplers = gpuShaderCompileResult->GetNumSamplers();
        shaderInfo.num_uniform_buffers = gpuShaderCompileResult->GetNumUniformBuffers();
        auto hash = StringUtils::StringToFullBlake3(dataOptional.value());
        Store(resourceRef, mtimeInt64, hash,
              gpuShaderCompileResult->GetCode().data(),
              gpuShaderCompileResult->GetCodeSize());
    } else {
        //Successfully read the cache.
        //成功读取缓存。
        shaderInfo.code = reinterpret_cast<const uint8_t *>(shaderCacheMessagePtr->spirvbinary().data());
        shaderInfo.code_size = shaderCacheMessagePtr->spirvbinary().size();
        shaderInfo.num_samplers = shaderCacheMessagePtr->numsamplers();
        shaderInfo.num_uniform_buffers = shaderCacheMessagePtr->numuniformbuffers();
    }
    if (shaderInfo.code_size % sizeof(unsigned int) != 0) {
        LogCat::w(std::source_location::current(), "Invalid SPIR-V input");
        return nullptr;
    }
    SDL_GPUShader *gpuShader = SDL_CreateGPUShader(device, &shaderInfo);
    if (gpuShader == nullptr) {
        LogCat::w(std::source_location::current(), "SDL_CreateGPUShader failed: ", SDL_GetError());
        return nullptr;
    }
    auto shaderResourceResult = std::make_shared<ShaderResourceResult>();
    shaderResourceResult->SetResource(gpuShader);
    shaderResourceResult->SetDevice(device);
    shaderResourceResult->SetResourcePack(resourcePack);
    return shaderResourceResult;
}

std::optional<std::string> glimmer::ShaderCache::ReadSource(const std::filesystem::path &path) {
    if (source_.has_value()) {
        //It has been read before, so do not read it again.
        //之前读取过，不要重复读取。
        return source_;
    }
    source_ = virtualFileSystem_->ReadFileAsString(path);
    return source_;
}

std::filesystem::path glimmer::ShaderCache::GetCacheFilePath(const ResourceRef *resourceRef) const {
    return (cacheDir_ / "shaders" / StringUtils::SanitizeFileName(resourceRef->GetPackageId()) /
            StringUtils::SanitizeFileName(resourceRef->GetResourceKey()));
}

std::unique_ptr<ShaderCacheMessage> glimmer::ShaderCache::TryLoad(int64_t mtime, const ResourceRef *resourceRef,
                                                                  const std::filesystem::path &sourcePath) {
    const std::filesystem::path cacheFilePath = GetCacheFilePath(resourceRef);
    if (!virtualFileSystem_->Exists(cacheFilePath)) {
        return nullptr;
    }
    const auto cacheData = virtualFileSystem_->ReadFileAsString(cacheFilePath);
    if (!cacheData.has_value()) {
        return nullptr;
    }
    auto cacheMessage = std::make_unique<ShaderCacheMessage>();
    if (!cacheMessage->ParseFromString(cacheData.value())) {
        //Corrupted or truncated cache: discard it and fall back to recompiling.
        //缓存损坏或被截断：丢弃缓存并回退到重新编译。
        LogCat::w(std::source_location::current(), "Shader cache corrupted, discarding: ", cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return nullptr;
    }
    if (cacheMessage->spirvbinary().empty()) {
        LogCat::w(std::source_location::current(), "Shader cache has no SPIR-V data, discarding: ",
                  cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return nullptr;
    }
    auto oldResourceRef = ResourceRef();
    oldResourceRef.ReadResourceRefMessage(cacheMessage->shaderresourceref());
    if (resourceRef->GetFingerprint() != oldResourceRef.GetFingerprint()) {
        //The cache file belongs to a different shader; do not trust it.
        //缓存文件属于其他着色器，不可信。
        LogCat::w(std::source_location::current(), "Shader cache resource mismatch, discarding: ",
                  cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return nullptr;
    }
    if (cacheMessage->sourcemtime() == mtime) {
        //The source file did not change: the cached binary is still valid.
        //源文件未变化：缓存的二进制仍然有效。
        LogCat::i("Shader cache hit (mtime): ", cacheFilePath.string());
        return cacheMessage;
    }
    //The modification time differs: confirm with the blake3 hash of the source.
    //修改时间不同：用源码的 blake3 哈希进一步确认。
    auto source = ReadSource(sourcePath);
    if (!source.has_value()) {
        return nullptr;
    }
    auto newBlake3 = StringUtils::StringToFullBlake3(source.value());
    if (!cacheMessage->blake3hash().empty()
        && std::memcmp(newBlake3.data(), cacheMessage->blake3hash().data(), BLAKE3_OUT_LEN) != 0) {
        LogCat::i("Shader cache hit (blake3): ", cacheFilePath.string());
        //Refresh the stored modification time so the next launch takes the
        //fast path again.
        //刷新缓存的修改时间，使下次启动重新走快速路径。
        Store(resourceRef, mtime, newBlake3, cacheMessage->spirvbinary().data(),
              cacheMessage->spirvbinary().size());
        return cacheMessage;
    }
    return nullptr;
}


void glimmer::ShaderCache::Store(const ResourceRef *resourceRef, int64_t mtime, const std::array<uint8_t, 32> &hash,
                                 const void *spirv, size_t spirvSize) const {
    if (resourceRef == nullptr || virtualFileSystem_ == nullptr || spirv == nullptr || spirvSize == 0) {
        return;
    }
    ShaderCacheMessage cacheMessage;
    cacheMessage.set_sourcemtime(mtime);
    std::string &dst = *cacheMessage.mutable_blake3hash();
    dst.assign(reinterpret_cast<const char *>(hash.data()), hash.size());
    resourceRef->WriteResourceRefMessage(*cacheMessage.mutable_shaderresourceref());
    cacheMessage.set_spirvbinary(spirv, spirvSize);
    const std::filesystem::path cacheFilePath = GetCacheFilePath(resourceRef);
    if (!virtualFileSystem_->WriteFile(cacheFilePath, cacheMessage.SerializeAsString())) {
        LogCat::w(std::source_location::current(), "Failed to write shader cache: ", cacheFilePath.string());
    }
}

glimmer::ShaderCache::~ShaderCache() noexcept = default;
