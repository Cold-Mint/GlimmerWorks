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
#include "LanguageFileProcessor.h"

#include "core/config/Constants.h"
#include "core/context/AppContext.h"

std::optional<std::string> glimmer::LanguageFileProcessor::GetDataType(const std::string &fileName) {
    const size_t lastDot = fileName.rfind('.');
    if (lastDot == std::string::npos) {
        return std::nullopt;
    }

    const size_t secondLastDot = fileName.rfind('.', lastDot - 1);
    if (secondLastDot == std::string::npos) {
        return std::nullopt;
    }

    if (const std::string format = fileName.substr(lastDot + 1); format != "toml") {
        return std::nullopt;
    }
    return fileName.substr(secondLastDot + 1,
                           lastDot - secondLastDot - 1);
}

std::optional<std::string>
glimmer::LanguageFileProcessor::ExtractLanguageFromFileName(const std::string_view fileName) {
    constexpr std::string_view suffix = ".strings.toml";
    if (!fileName.ends_with(suffix)) {
        return std::nullopt;
    }
    std::string base(fileName.substr(0, fileName.size() - suffix.size()));
    auto pos = base.rfind('.');
    if (pos == std::string::npos) {
        return base;
    }
    return base.substr(pos + 1);
}

bool glimmer::LanguageFileProcessor::ProcessLanguageFile(const std::filesystem::path &file,
                                                         const std::string_view dataType,
                                                         const std::string_view fileName,
                                                         std::vector<std::filesystem::path> &defaultLanguageFiles,
                                                         std::vector<std::filesystem::path> &targetLanguageFiles,
                                                         const AppContext *appContext) {
    if (dataType != DATA_FILE_TYPE_STRINGS) {
        return false;
    }
    const auto langOptional = ExtractLanguageFromFileName(fileName);
    if (!langOptional.has_value()) {
        return true;
    }
    if (const auto &fileLang = langOptional.value(); fileLang == appContext->GetLanguage()) {
        targetLanguageFiles.push_back(file);
    } else if (fileLang == "default") {
        defaultLanguageFiles.push_back(file);
    }
    return true;
}
