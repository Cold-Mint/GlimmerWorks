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
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace glimmer {
    /**
     * File provider
     * 文件提供者
     */
    class IFileProvider {
    public:
        virtual ~IFileProvider() = default;

        /**
         * Get the name of the file provider
         * 获取文件提供者的名称
         * @return
         */
        [[nodiscard]] virtual std::string GetFileProviderName() const = 0;


        /**
         * Get File Or Folder Name
         * 获取文件或文件夹名称
         * @param path path 路径
         * @return
         */
        [[nodiscard]] virtual std::optional<std::string> GetFileOrFolderName(const std::string &path) const = 0;

        /**
         * Read the file
         * 读取文件
         * @param path path 文件路径
         * @return string 文件内容
         */
        [[nodiscard]] virtual std::optional<std::string> ReadFile(const std::string &path) = 0;


        /**
         *Delete File Or Folder
         * 删除文件或者文件夹
         * @return
         */
        [[nodiscard]] virtual bool DeleteFileOrFolder(const std::string &path) = 0;

        /**
         * Read Stream
         * 读取文件流
         * @param path  path 文件路径
         * @return stream 文件流
         */
        [[nodiscard]] virtual std::optional<std::unique_ptr<std::istream> > ReadStream(const std::string &path) = 0;


        /**
         * Whether the specified route exists
         * 指定的路经是否存在
         * @param path path 文件路径
         * @return 是否存在
         */
        [[nodiscard]] virtual bool Exists(const std::string &path) = 0;


        /**
         * IsFile
         * 指定的路径是否为文件
         * @param path path 文件路径
         * @return 是否为文件
         */
        [[nodiscard]] virtual bool IsFile(const std::string &path) = 0;

        /**
         * Write the file
         * 写出文件
         * @param path path 文件路径
         * @param content content文件内容
         * @return 是否写出成功
         */
        [[nodiscard]] virtual bool WriteFile(const std::string &path, const std::string &content) = 0;

        /**
         * List the files in the folder
         * 列出文件夹下的文件
         * @param path path 路径
         * @param recursive recursive Is it recursive? 是否递归
         * @return
         */
        [[nodiscard]] virtual std::vector<std::string> ListFile(const std::string &path, bool recursive) = 0;

        [[nodiscard]] virtual std::optional<std::string> GetParentPath(const std::string &path) const = 0;

        [[nodiscard]] virtual std::optional<std::string> GetActualPath(const std::string &path) const = 0;

        virtual bool CreateFolder(const std::string &path) = 0;
    };
}
