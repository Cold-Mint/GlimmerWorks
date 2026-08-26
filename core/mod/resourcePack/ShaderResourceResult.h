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
#include <filesystem>
#include <string>

namespace glimmer {
    /**
     * ShaderResourceResult
     * 着色器资源查找结果
     *
     * Holds the resolved shader file path (inside the resource pack that
     * provided it) together with the GLSL source code read from that file.
     * The path is kept so callers can inspect the file modification time
     * (used by the shader disk cache).
     * 保存解析出的着色器文件路径（位于提供它的材质包内）以及从该文件读取的
     * GLSL 源码。保留路径是为了让调用方可以查看文件修改时间（着色器磁盘缓存使用）。
     */
    class ShaderResourceResult {
        std::filesystem::path shaderPath_;
        std::string source_;

    public:
        void SetPath(const std::filesystem::path &shaderPath);

        [[nodiscard]] const std::filesystem::path &GetPath() const;

        void SetSource(std::string source);

        [[nodiscard]] const std::string &GetSource() const;
    };
}
