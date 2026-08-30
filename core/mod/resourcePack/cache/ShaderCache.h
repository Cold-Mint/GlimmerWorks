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
#include "blake3.h"
#include "core/mod/resourcePack/BaseResourceCache.h"
#include "src/cache/shader_cache.pb.h"

namespace glimmer {
    class GpuShaderCompileResult;

    class ShaderCache : public BaseResourceCache<ShaderResourceResult> {
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        std::filesystem::path cacheDir_;
        //Shader source code
        //着色器源码
        std::optional<std::string> source_;

    protected:
        std::shared_ptr<ShaderResourceResult> LoadResourceFromPack(AppContext *appContext,
                                                                   const ResourceRef *resourceRef,
                                                                   const ResourcePack *resourcePack) override;

        /**
         * Reading the shader source code
         * 读取着色器源码
         * @param path
         */
        std::optional<std::string> ReadSource(const std::filesystem::path &path);


        /**
         * Build the cache file path for a shader resource reference
         * (<cachePath>/shaders/<packageId>/<resourceKey>.cache, sanitized).
         * 构造着色器资源引用对应的缓存文件路径
         * （<cachePath>/shaders/<packageId>/<resourceKey>.cache，已过滤非法字符）。
         */
        [[nodiscard]] std::filesystem::path GetCacheFilePath(const ResourceRef *resourceRef) const;

        /**
         * Try to load a valid cached SPIR-V binary for the given shader.
         * 尝试为给定着色器加载有效的缓存 SPIR-V 二进制。
         * @param mtime mtime 文件修改时间
         * @param resourceRef resourceRef 着色器资源引用
         * @param sourcePath sourcePath 着色器源文件路径（用于读取修改时间）
         * @return The SPIR-V binary on a cache hit, std::nullopt on a miss
         * (missing/corrupted/stale cache).
         * 命中时返回 SPIR-V 二进制；未命中（缓存缺失/损坏/过期）返回 std::nullopt。
         */
        [[nodiscard]] std::unique_ptr<ShaderCacheMessage> TryLoad(int64_t mtime,
                                                                  const ResourceRef *resourceRef,
                                                                  const std::filesystem::path &sourcePath);

        /**
         * Store a freshly compiled SPIR-V binary in the disk cache.
         * 把刚编译的 SPIR-V 二进制写入磁盘缓存。
         * @param resourceRef resourceRef 着色器资源引用
         * @param mtime mtime 源码修改时间
         * @param hash hash 着色器哈希
         * @param spirv spirv SPIR-V 二进制数据
         * @param spirvSize spirvSize 数据字节数
         */
        void Store(const ResourceRef *resourceRef, int64_t mtime,
                   const std::array<uint8_t,BLAKE3_OUT_LEN> &hash,
                   const void *spirv, size_t spirvSize) const;

    public:
        ~ShaderCache() noexcept override;
    };
}
