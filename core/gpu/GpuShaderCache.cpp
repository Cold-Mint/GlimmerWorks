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
#include "GpuShaderCache.h"

#include <system_error>

#include "blake3.h"
#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/utils/StringUtils.h"
#include "core/vfs/VirtualFileSystem.h"
#include "src/cache/shader_cache.pb.h"

glimmer::GpuShaderCache::GpuShaderCache(VirtualFileSystem *virtualFileSystem,
                                        const std::filesystem::path &cachePath)
    : virtualFileSystem_(virtualFileSystem), cacheDir_(cachePath / "shaders") {
}

std::filesystem::path glimmer::GpuShaderCache::GetCacheFilePath(const ResourceRef *resourceRef) const {
    return cacheDir_ / StringUtils::SanitizeFileName(resourceRef->GetPackageId()) /
           (StringUtils::SanitizeFileName(resourceRef->GetResourceKey()) + ".cache");
}

std::optional<int64_t> glimmer::GpuShaderCache::GetSourceMtime(const std::filesystem::path &sourcePath) const {
    std::filesystem::path actualPath = sourcePath;
    if (virtualFileSystem_ != nullptr) {
        if (auto resolved = virtualFileSystem_->GetActualPath(sourcePath); resolved.has_value()) {
            actualPath = std::move(resolved.value());
        }
    }
    std::error_code errorCode;
    const auto fileTime = std::filesystem::last_write_time(actualPath, errorCode);
    if (errorCode) {
        return std::nullopt;
    }
    return static_cast<int64_t>(fileTime.time_since_epoch().count());
}

std::string glimmer::GpuShaderCache::ComputeBlake3(const std::string &source) {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, source.data(), source.size());
    std::string hash(BLAKE3_OUT_LEN, '\0');
    blake3_hasher_finalize(&hasher, reinterpret_cast<uint8_t *>(hash.data()), BLAKE3_OUT_LEN);
    return hash;
}

std::optional<std::string> glimmer::GpuShaderCache::TryLoad(const ResourceRef *resourceRef,
                                                            const std::filesystem::path &sourcePath,
                                                            const std::string &source) const {
    if (resourceRef == nullptr || virtualFileSystem_ == nullptr) {
        return std::nullopt;
    }
    const auto sourceMtime = GetSourceMtime(sourcePath);
    if (!sourceMtime.has_value()) {
        //Without the source modification time the cache cannot be validated.
        //无法获取源文件修改时间，缓存无法校验。
        return std::nullopt;
    }
    const std::filesystem::path cacheFilePath = GetCacheFilePath(resourceRef);
    if (!virtualFileSystem_->Exists(cacheFilePath)) {
        return std::nullopt;
    }
    const auto cacheData = virtualFileSystem_->ReadFileAsString(cacheFilePath);
    if (!cacheData.has_value()) {
        return std::nullopt;
    }
    ShaderCacheMessage cacheMessage;
    if (!cacheMessage.ParseFromString(cacheData.value())) {
        //Corrupted or truncated cache: discard it and fall back to recompiling.
        //缓存损坏或被截断：丢弃缓存并回退到重新编译。
        LogCat::w(std::source_location::current(), "Shader cache corrupted, discarding: ", cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return std::nullopt;
    }
    if (cacheMessage.spirvbinary().empty()) {
        LogCat::w(std::source_location::current(), "Shader cache has no SPIR-V data, discarding: ",
                  cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return std::nullopt;
    }
    if (cacheMessage.has_shaderresourceref() &&
        (cacheMessage.shaderresourceref().packid() != resourceRef->GetPackageId() ||
         cacheMessage.shaderresourceref().resourcekey() != resourceRef->GetResourceKey() ||
         cacheMessage.shaderresourceref().resourcetype() != RESOURCE_SHADER)) {
        //The cache file belongs to a different shader; do not trust it.
        //缓存文件属于其他着色器，不可信。
        LogCat::w(std::source_location::current(), "Shader cache resource mismatch, discarding: ",
                  cacheFilePath.string());
        static_cast<void>(virtualFileSystem_->DeleteFileOrFolder(cacheFilePath));
        return std::nullopt;
    }
    if (cacheMessage.sourcemtime() == sourceMtime.value()) {
        //The source file did not change: the cached binary is still valid.
        //源文件未变化：缓存的二进制仍然有效。
        LogCat::i("Shader cache hit (mtime): ", cacheFilePath.string());
        return cacheMessage.spirvbinary();
    }
    //The modification time differs: confirm with the blake3 hash of the source.
    //修改时间不同：用源码的 blake3 哈希进一步确认。
    if (!cacheMessage.blake3hash().empty() && cacheMessage.blake3hash() == ComputeBlake3(source)) {
        LogCat::i("Shader cache hit (blake3): ", cacheFilePath.string());
        //Refresh the stored modification time so the next launch takes the
        //fast path again.
        //刷新缓存的修改时间，使下次启动重新走快速路径。
        Store(resourceRef, sourcePath, source, cacheMessage.spirvbinary().data(), cacheMessage.spirvbinary().size());
        return cacheMessage.spirvbinary();
    }
    //The source file changed: the cached binary is stale.
    //源文件已变化：缓存二进制已过期。
    return std::nullopt;
}

void glimmer::GpuShaderCache::Store(const ResourceRef *resourceRef, const std::filesystem::path &sourcePath,
                                    const std::string &source, const void *spirv, const size_t spirvSize) const {
    if (resourceRef == nullptr || virtualFileSystem_ == nullptr || spirv == nullptr || spirvSize == 0) {
        return;
    }
    ShaderCacheMessage cacheMessage;
    cacheMessage.set_sourcemtime(GetSourceMtime(sourcePath).value_or(0));
    cacheMessage.set_blake3hash(ComputeBlake3(source));
    resourceRef->WriteResourceRefMessage(*cacheMessage.mutable_shaderresourceref());
    cacheMessage.set_spirvbinary(spirv, spirvSize);
    const std::filesystem::path cacheFilePath = GetCacheFilePath(resourceRef);
    if (!virtualFileSystem_->WriteFile(cacheFilePath, cacheMessage.SerializeAsString())) {
        LogCat::w(std::source_location::current(), "Failed to write shader cache: ", cacheFilePath.string());
    }
}
