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

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>

namespace glimmer {
    class ResourceRef;
    class VirtualFileSystem;

    /**
     * GpuShaderCache
     * GPU 着色器磁盘缓存
     *
     * Caches compiled SPIR-V binaries on disk so shaders located through the
     * ResourceLocator do not need to be recompiled with glslang on every
     * launch. The on-disk format is ShaderCacheMessage (cache/shader_cache.proto):
     * the source file modification time, the blake3 hash of the source, the
     * shader resource reference and the compiled SPIR-V binary.
     * 把编译好的 SPIR-V 二进制缓存到磁盘，使通过资源定位器找到的着色器不必每次
     * 启动都用 glslang 重新编译。磁盘格式为 ShaderCacheMessage
     * （cache/shader_cache.proto）：源文件修改时间、源码 blake3 哈希、
     * 着色器资源引用以及编译后的 SPIR-V 二进制。
     *
     * Validation rule: if the source modification time matches the cached one
     * the cache is used directly; if it differs, the source blake3 hash is
     * computed and compared with the cached hash. Corrupted, truncated or
     * hash-mismatched cache files are silently discarded (the caller falls
     * back to recompiling); this class never throws.
     * 校验规则：源文件修改时间与缓存一致时直接使用缓存；不一致时计算源码
     * blake3 哈希并与缓存哈希比较。损坏、截断或哈希不匹配的缓存文件会被直接
     * 丢弃（调用方回退到重新编译）；本类绝不抛异常。
     */
    class GpuShaderCache {
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        std::filesystem::path cacheDir_;

        /**
         * Build the cache file path for a shader resource reference
         * (<cachePath>/shaders/<packageId>/<resourceKey>.cache, sanitized).
         * 构造着色器资源引用对应的缓存文件路径
         * （<cachePath>/shaders/<packageId>/<resourceKey>.cache，已过滤非法字符）。
         */
        [[nodiscard]] std::filesystem::path GetCacheFilePath(const ResourceRef *resourceRef) const;

        /**
         * Read the source file modification time (nanosecond ticks since the
         * file clock epoch), std::nullopt when it cannot be determined.
         * 读取源文件修改时间（文件时钟纪元起的纳秒计数），无法获取时返回 std::nullopt。
         */
        [[nodiscard]] std::optional<int64_t> GetSourceMtime(const std::filesystem::path &sourcePath) const;

        /**
         * Compute the blake3 hash (32 raw bytes) of the given source code.
         * 计算给定源码的 blake3 哈希（32 字节原始数据）。
         */
        [[nodiscard]] static std::string ComputeBlake3(const std::string &source);

    public:
        GpuShaderCache(VirtualFileSystem *virtualFileSystem, const std::filesystem::path &cachePath);

        /**
         * Try to load a valid cached SPIR-V binary for the given shader.
         * 尝试为给定着色器加载有效的缓存 SPIR-V 二进制。
         * @param resourceRef resourceRef 着色器资源引用
         * @param sourcePath sourcePath 着色器源文件路径（用于读取修改时间）
         * @param source source 着色器源码（修改时间变化时用于 blake3 校验）
         * @return The SPIR-V binary on a cache hit, std::nullopt on a miss
         * (missing/corrupted/stale cache).
         * 命中时返回 SPIR-V 二进制；未命中（缓存缺失/损坏/过期）返回 std::nullopt。
         */
        [[nodiscard]] std::optional<std::string> TryLoad(const ResourceRef *resourceRef,
                                                         const std::filesystem::path &sourcePath,
                                                         const std::string &source) const;

        /**
         * Store a freshly compiled SPIR-V binary in the disk cache.
         * 把刚编译的 SPIR-V 二进制写入磁盘缓存。
         * @param resourceRef resourceRef 着色器资源引用
         * @param sourcePath sourcePath 着色器源文件路径
         * @param source source 着色器源码
         * @param spirv spirv SPIR-V 二进制数据
         * @param spirvSize spirvSize 数据字节数
         */
        void Store(const ResourceRef *resourceRef, const std::filesystem::path &sourcePath, const std::string &source,
                   const void *spirv, size_t spirvSize) const;
    };
}
