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
#include <filesystem>
#include <memory>

#include "FileProvider.h"

namespace glimmer
{
    /**
     * File provider implemented based on the standard library
     * 基于标准库实现的文件提供者
     */
    class StdFileProvider : public IFileProvider
    {
        std::filesystem::path root_;
        std::string name_ = "std_file_provider";

        /**
         * GetFullPath
         * 获取完整路径
         * @param relativePath relativePath 相对路径
         * @return
         */
        [[nodiscard]] std::optional<std::filesystem::path> GetFullPath(const std::filesystem::path& relativePath) const;

    public:
        ~StdFileProvider() override = default;

        explicit StdFileProvider(std::string rootPath);

        [[nodiscard]] bool DeleteFileOrFolder(const std::filesystem::path& path) override;

        [[nodiscard]] std::unique_ptr<std::istream> ReadStream(const std::filesystem::path& path) override;

        [[nodiscard]] std::vector<std::filesystem::path>
        ListFile(const std::filesystem::path& path, bool recursive) override;

        [[nodiscard]] std::optional<std::filesystem::path>
        GetParentPath(const std::filesystem::path& path) const override;

        [[nodiscard]] std::optional<std::filesystem::path>
        GetActualPath(const std::filesystem::path& path) const override;

        [[nodiscard]] bool CreateFolder(const std::filesystem::path& path) override;

        [[nodiscard]] std::string_view GetFileProviderName() const override;


        [[nodiscard]] std::optional<std::string> GetFileOrFolderName(const std::filesystem::path& path) const override;

        [[nodiscard]] bool Exists(const std::filesystem::path& path) override;

        [[nodiscard]] bool IsFile(const std::filesystem::path& path) override;

        [[nodiscard]] bool WriteFile(const std::filesystem::path& path, const std::string& content) const override;
    };
}
