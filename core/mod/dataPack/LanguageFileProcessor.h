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
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace glimmer {
    class AppContext;

    /**
     * LanguageFileProcessor
     * 语言文件处理器
     * Stateless helpers for classifying data-pack files and routing language (.strings.toml) files.
     * 无状态工具，用于识别数据包文件类型并路由语言（.strings.toml）文件。
     */
    class LanguageFileProcessor {
    public:
        /**
         * GetDataType
         * 获取数据类型
         * Extracts the data type from a "xxx.<type>.toml" file name.
         * 从「xxx.类型.toml」文件名中提取数据类型。
         * @param fileName fileName 文件名
         * @return The data type, or nullopt if unrecognized 数据类型，无法识别时返回nullopt
         */
        static std::optional<std::string> GetDataType(const std::string &fileName);

        /**
         * ExtractLanguageFromFileName
         * 从文件名提取语言
         * @param fileName fileName 文件名
         * @return The language code, or nullopt if not a language file 语言代码，非语言文件返回nullopt
         */
        static std::optional<std::string> ExtractLanguageFromFileName(std::string_view fileName);

        /**
         * ProcessLanguageFile
         * 处理语言文件
         * Routes a language file into the default or target language file list.
         * 将语言文件归类到默认语言或目标语言文件列表。
         * @param file file 文件路径
         * @param dataType dataType 数据类型
         * @param fileName fileName 文件名
         * @param defaultLanguageFiles defaultLanguageFiles 默认语言文件列表
         * @param targetLanguageFiles targetLanguageFiles 目标语言文件列表
         * @param appContext appContext 应用上下文
         * @return True if the file was a language file 该文件是否为语言文件
         */
        static bool ProcessLanguageFile(const std::filesystem::path &file, std::string_view dataType,
                                        std::string_view fileName,
                                        std::vector<std::filesystem::path> &defaultLanguageFiles,
                                        std::vector<std::filesystem::path> &targetLanguageFiles,
                                        const AppContext *appContext);
    };
}
