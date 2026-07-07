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
#include "VirtualFileSystem.h"

#include <fstream>

#include "core/utils/StringUtils.h"

void glimmer::VirtualFileSystem::Mount(std::unique_ptr<IFileProvider> provider)
{
    fileProviders_.emplace_back(std::move(provider));
}

std::string glimmer::VirtualFileSystem::ListMounts() const
{
    std::stringstream result;
    bool hasContent = false;
    for (const auto& provider : fileProviders_)
    {
        if (hasContent)
        {
            result << "\n";
        }
        result << provider->GetFileProviderName();
        hasContent = true;
    }
    return result.str();
}

bool glimmer::VirtualFileSystem::Exists(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (provider->Exists(path))
        {
            return true;
        }
    }
    return false;
}

bool glimmer::VirtualFileSystem::CreateFolder(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (provider->CreateFolder(path))
        {
            return true;
        }
    }
    return false;
}

bool glimmer::VirtualFileSystem::IsFile(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (provider->IsFile(path))
        {
            return true;
        }
    }
    return false;
}

std::optional<std::string> glimmer::VirtualFileSystem::GetFileOrFolderName(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (auto fileName = provider->GetFileOrFolderName(path); fileName.has_value())
        {
            return fileName;
        }
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> glimmer::VirtualFileSystem::GetParentPath(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (auto parentPath = provider->GetParentPath(path); parentPath.has_value())
        {
            return parentPath;
        }
    }
    return std::nullopt;
}


bool glimmer::VirtualFileSystem::WriteFile(const std::string& path, const std::string& content) const
{
    for (const auto& provider : fileProviders_)
    {
        if (provider->WriteFile(path, content))
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<std::istream> glimmer::VirtualFileSystem::ReadFileAsStream(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (std::unique_ptr<std::istream> stream = provider->ReadStream(path); stream != nullptr)
        {
            return stream;
        }
    }
    return nullptr;
}

std::optional<std::string> glimmer::VirtualFileSystem::ReadFileAsString(const std::filesystem::path& path) const
{
    const std::unique_ptr<std::istream> streamUniquePtr = ReadFileAsStream(path);
    if (streamUniquePtr == nullptr)
    {
        return std::nullopt;
    }
    const auto stream = streamUniquePtr.get();
    if (stream == nullptr)
    {
        return std::nullopt;
    }
    return StringUtils::StreamToString(stream);
}


std::vector<std::filesystem::path> glimmer::VirtualFileSystem::ListFile(const std::filesystem::path& path,
                                                                        const bool recursive) const
{
    for (const auto& provider : fileProviders_)
    {
        if (auto content = provider->ListFile(path, recursive); !content.empty())
        {
            return content;
        }
    }
    return {};
}

bool glimmer::VirtualFileSystem::DeleteFileOrFolder(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        if (provider->DeleteFileOrFolder(path))
        {
            return true;
        }
    }
    return false;
}

std::optional<std::filesystem::path> glimmer::VirtualFileSystem::GetActualPath(const std::filesystem::path& path) const
{
    for (const auto& provider : fileProviders_)
    {
        auto actualPath = provider->GetActualPath(path);
        if (actualPath.has_value())
        {
            return actualPath;
        }
    }
    return std::nullopt;
}
