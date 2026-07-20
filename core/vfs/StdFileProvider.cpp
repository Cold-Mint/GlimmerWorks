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

#include "core/log/LogCat.h"


std::optional<std::filesystem::path> glimmer::StdFileProvider::GetFullPath(
    const std::filesystem::path& relativePath) const
{
    std::filesystem::path fullPath = root_ / relativePath;
    auto absFull = std::filesystem::weakly_canonical(std::filesystem::absolute(fullPath));
    auto absRoot = std::filesystem::weakly_canonical(std::filesystem::absolute(root_));
    const std::string fullStr = absFull.string();
    const std::string rootStr = absRoot.string();
    if (!(fullStr == rootStr || fullStr.starts_with(
        rootStr + std::string(1, std::filesystem::path::preferred_separator))))
    {
        LogCat::e(std::source_location::current(), "Access to unmounted directories is prohibited. fullPath(", fullPath,
                  ")rootPath(", rootStr, ")");
        return std::nullopt;
    }
    return fullPath;
}

glimmer::StdFileProvider::StdFileProvider(std::string rootPath)
    : root_(std::move(rootPath))
{
}

bool glimmer::StdFileProvider::DeleteFileOrFolder(const std::filesystem::path& path)
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return false;
    }
    const std::filesystem::path& fullPath = fullPathOptional.value();
    if (IsFile(path))
    {
        return std::filesystem::remove(fullPath);
    }
    return std::filesystem::remove_all(fullPath) > 0;
}

std::unique_ptr<std::istream> glimmer::StdFileProvider::ReadStream(const std::filesystem::path& path)
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return nullptr;
    }
    const std::filesystem::path& fullPath = fullPathOptional.value();
    if (!std::filesystem::exists(fullPath) || !std::filesystem::is_regular_file(fullPath))
    {
        return nullptr;
    }
    auto file = std::make_unique<std::ifstream>(fullPath, std::ios::binary);
    if (!file->is_open())
    {
        return nullptr;
    }
    return file;
}

std::vector<std::filesystem::path> glimmer::StdFileProvider::ListFile(const std::filesystem::path& path, bool recursive)
{
    std::vector<std::filesystem::path> result;
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return result;
    }
    const std::filesystem::path& dir = fullPathOptional.value();
    std::error_code errorCode;
    if (!std::filesystem::exists(dir, errorCode) ||
        !std::filesystem::is_directory(dir, errorCode))
    {
        return result;
    }
    if (recursive)
    {
        for (std::filesystem::recursive_directory_iterator it(dir, errorCode), end;
             it != end;
             it.increment(errorCode))
        {
            if (errorCode.value() != 0)
            {
                continue;
            }

            const auto relative =
                std::filesystem::relative(it->path(), root_, errorCode);

            if (errorCode.value() == 0)
            {
                result.push_back(relative);
            }
        }
    }
    else
    {
        for (std::filesystem::directory_iterator it(dir, errorCode), end;
             it != end;
             it.increment(errorCode))
        {
            if (errorCode.value() != 0)
            {
                continue;
            }

            const auto relative =
                std::filesystem::relative(it->path(), root_, errorCode);

            if (errorCode.value() == 0)
            {
                result.push_back(relative);
            }
        }
    }
    return result;
}

std::optional<std::filesystem::path> glimmer::StdFileProvider::GetParentPath(const std::filesystem::path& path) const
{
    return std::filesystem::path(path).parent_path();
}

std::optional<std::filesystem::path> glimmer::StdFileProvider::GetActualPath(const std::filesystem::path& path) const
{
    return std::filesystem::path(root_) / path;
}


bool glimmer::StdFileProvider::CreateFolder(const std::filesystem::path& path)
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return false;
    }
    const std::filesystem::path& fullPath = fullPathOptional.value();
    std::error_code errorCode;
    std::filesystem::create_directories(fullPath, errorCode);
    return errorCode.value() == 0;
}

std::string_view glimmer::StdFileProvider::GetFileProviderName() const
{
    return name_;
}

std::optional<std::string> glimmer::StdFileProvider::GetFileOrFolderName(const std::filesystem::path& path) const
{
    return std::filesystem::path(path).filename().string();
}

bool glimmer::StdFileProvider::Exists(const std::filesystem::path& path)
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return false;
    }
    return std::filesystem::exists(fullPathOptional.value());
}

bool glimmer::StdFileProvider::IsFile(const std::filesystem::path& path)
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return false;
    }
    return std::filesystem::is_regular_file(fullPathOptional.value());
}

bool glimmer::StdFileProvider::WriteFile(const std::filesystem::path& path, const std::string& content) const
{
    const std::optional<std::filesystem::path> fullPathOptional = GetFullPath(path);
    if (!fullPathOptional.has_value())
    {
        return false;
    }
    const std::filesystem::path& fullPath = fullPathOptional.value();
    std::filesystem::create_directories(fullPath.parent_path());
    std::ofstream file(fullPath, std::ios::binary);
    if (!file) return false;

    file.write(content.data(), static_cast<int>(content.size()));
    return file.good();
}
