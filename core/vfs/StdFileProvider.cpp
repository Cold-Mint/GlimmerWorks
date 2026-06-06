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
#include "StdFileProvider.h"

#include <filesystem>
#include <fstream>

#include "../log/LogCat.h"


glimmer::StdFileProvider::StdFileProvider(std::string rootPath)
    : root_(std::move(rootPath)) {
}

bool glimmer::StdFileProvider::DeleteFileOrFolder(const std::string &path) {
    const std::filesystem::path fullPath = root_ / path;
    if (IsFile(path)) {
        return std::filesystem::remove(fullPath);
    }
    return std::filesystem::remove_all(fullPath);
}

std::vector<std::string> glimmer::StdFileProvider::ListFile(const std::string &path, bool recursive) {
    std::vector<std::string> result;

    const std::filesystem::path dir = root_ / path;

    std::error_code ec;

    if (!std::filesystem::exists(dir, ec) ||
        !std::filesystem::is_directory(dir, ec)) {
        return result;
    }

    if (recursive) {
        for (std::filesystem::recursive_directory_iterator it(dir, ec), end;
             it != end;
             it.increment(ec)) {
            if (ec) continue;

            const auto relative =
                    std::filesystem::relative(it->path(), root_, ec);

            if (!ec) {
                result.push_back(relative.generic_string());
            }
        }
    } else {
        for (std::filesystem::directory_iterator it(dir, ec), end;
             it != end;
             it.increment(ec)) {
            if (ec) continue;

            const auto relative =
                    std::filesystem::relative(it->path(), root_, ec);

            if (!ec) {
                result.push_back(relative.generic_string());
            }
        }
    }

    return result;
}


std::string glimmer::StdFileProvider::GetFileProviderName() const {
    return "StdFileProvider(" + root_.string() + ")";
}

std::optional<std::string> glimmer::StdFileProvider::ReadFile(const std::string &path) {
    std::filesystem::path fullPath = root_ / path;
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath)) {
        return std::nullopt;
    }

    std::ifstream file(fullPath, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    std::string content((std::istreambuf_iterator(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

std::optional<std::string> glimmer::StdFileProvider::GetFileOrFolderName(const std::string &path) const {
    return std::filesystem::path(path).filename().string();
}

std::optional<std::unique_ptr<std::istream> > glimmer::StdFileProvider::ReadStream(const std::string &path) {
    std::filesystem::path fullPath = root_ / path;
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath)) {
        return std::nullopt;
    }
    auto file = std::make_unique<std::ifstream>(fullPath, std::ios::binary);
    if (!file->is_open()) {
        return std::nullopt;
    }
    return file;
}

bool glimmer::StdFileProvider::Exists(const std::string &path) {
    return std::filesystem::exists(root_ / path);
}

bool glimmer::StdFileProvider::IsFile(const std::string &path) {
    return std::filesystem::is_regular_file(root_ / path);
}

bool glimmer::StdFileProvider::WriteFile(const std::string &path, const std::string &content) {
    const std::filesystem::path fullPath = root_ / path;
    std::filesystem::create_directories(fullPath.parent_path());

    std::ofstream file(fullPath, std::ios::binary);
    if (!file) return false;

    file.write(content.data(), static_cast<int>(content.size()));
    return file.good();
}

bool glimmer::StdFileProvider::CreateFolder(const std::string &path) {
    const std::filesystem::path fullPath = root_ / path;
    try {
        return std::filesystem::create_directories(fullPath);
    } catch (const std::filesystem::filesystem_error &e) {
        LogCat::e("Failed to create folder: ", fullPath.string(), " Error: ", e.what());
        return false;
    }
}

std::optional<std::string> glimmer::StdFileProvider::GetParentPath(const std::string &path) const {
    auto parent = std::filesystem::path(path).parent_path();
if (parent.empty()) {
    return std::nullopt;
}
return parent.string();
}

std::optional<std::string> glimmer::StdFileProvider::GetActualPath(const std::string &path) const {
    std::filesystem::path fullPath = std::filesystem::path(root_) / path;
    return std::make_optional(fullPath.string());
}
