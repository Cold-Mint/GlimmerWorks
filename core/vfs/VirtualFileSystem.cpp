//
// Created by Cold-Mint on 2025/11/28.
//

#include "VirtualFileSystem.h"

#include <fstream>
#include "../log/LogCat.h"

void glimmer::VirtualFileSystem::Mount(std::unique_ptr<FileProvider> provider) {
    LogCat::i("VirtualFileSystem Mount FileProvider =", provider->GetFileProviderName());
    fileProviders_.push_back(std::move(provider));
}

std::string glimmer::VirtualFileSystem::ListMounts() const {
    LogCat::i("VirtualFileSystem ListMounts");

    std::string result;

    for (const auto &provider: fileProviders_) {
        if (!result.empty()) {
            result += "\n"; // 每个 provider 一行
        }
        result += provider->GetFileProviderName();
    }

    return result;
}

std::optional<std::string> glimmer::VirtualFileSystem::ReadFile(const std::string &path) const {
    LogCat::i("VirtualFileSystem ReadFile =", path);
    for (auto &provider: fileProviders_) {
        if (provider->Exists(path) && provider->IsFile(path)) {
            if (auto content = provider->ReadFile(path))
                return content;
        }
    }
    return std::nullopt;
}

bool glimmer::VirtualFileSystem::Exists(const std::string &path) const {
    LogCat::i("VirtualFileSystem Exists =", path);
    for (auto &provider: fileProviders_)
        if (provider->Exists(path))
            return true;
    return false;
}

bool glimmer::VirtualFileSystem::CreateFolder(const std::string &path) const {
    LogCat::i("VirtualFileSystem CreateFolder =", path);
    for (auto &provider: fileProviders_)
        if (provider->CreateFolder(path))
            return true;
    return false;
}

bool glimmer::VirtualFileSystem::IsFile(const std::string &path) const {
    LogCat::i("VirtualFileSystem IsFile =", path);
    for (auto &provider: fileProviders_)
        if (provider->IsFile(path))
            return true;
    return false;
}

std::optional<std::string> glimmer::VirtualFileSystem::GetFileOrFolderName(const std::string &path) const {
    LogCat::i("VirtualFileSystem GetFileName =", path);
    for (auto &provider: fileProviders_) {
        if (auto fileName = provider->GetFileOrFolderName(path); fileName.has_value()) {
            return fileName;
        }
    }
    return std::nullopt;
}

bool glimmer::VirtualFileSystem::WriteFile(const std::string &path, const std::string &content) const {
    LogCat::i("VirtualFileSystem WriteFile =", path);
    for (auto &provider: fileProviders_) {
        if (provider->WriteFile(path, content))
            return true;
    }
    return false;
}

std::optional<std::unique_ptr<std::istream> > glimmer::VirtualFileSystem::ReadStream(const std::string &path) const {
    LogCat::i("VirtualFileSystem ReadStream =", path);
    for (auto &provider: fileProviders_) {
        if (provider->Exists(path) && provider->IsFile(path)) {
            if (std::optional<std::unique_ptr<std::istream> > stream = provider->ReadStream(path); stream.
                has_value()) {
                return stream;
            }
        }
    }
    return std::nullopt;
}

std::vector<std::string> glimmer::VirtualFileSystem::ListFile(const std::string &path, bool recursive) const {
    LogCat::i("VirtualFileSystem ListFile =", path);
    for (auto &provider: fileProviders_) {
        if (provider->Exists(path) && !provider->IsFile(path)) {
            if (auto content = provider->ListFile(path, recursive); !content.empty()) {
                return content;
            }
        }
    }
    return {};
}

bool glimmer::VirtualFileSystem::DeleteFileOrFolder(const std::string &path) const {
    LogCat::i("VirtualFileSystem DeleteFileOrFolder =", path);
    for (auto &provider: fileProviders_) {
        if (provider->DeleteFileOrFolder(path)) {
            return true;
        }
    }
    return false;
}

std::optional<std::string> glimmer::VirtualFileSystem::GetActualPath(const std::string &path) const {
    LogCat::i("VirtualFileSystem GetActualPath =", path);
    for (auto &provider: fileProviders_) {
        auto actualPath = provider->GetActualPath(path);
        if (actualPath.has_value()) {
            return actualPath;
        }
    }
    return std::nullopt;
}
