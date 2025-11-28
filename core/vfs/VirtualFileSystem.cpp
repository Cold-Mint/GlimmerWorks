//
// Created by coldmint on 2025/11/28.
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

bool glimmer::VirtualFileSystem::WriteFile(const std::string &path, const std::string &content) const {
    LogCat::i("VirtualFileSystem WriteFile =", path);
    for (auto &provider: fileProviders_) {
        if (provider->WriteFile(path, content))
            return true;
    }
    return false;
}

std::optional<std::unique_ptr<std::ifstream> > glimmer::VirtualFileSystem::ReadStream(const std::string &path) const {
    LogCat::i("VirtualFileSystem ReadStream =", path);
    for (auto &provider: fileProviders_) {
        if (provider->Exists(path) && provider->IsFile(path)) {
            if (std::optional<std::unique_ptr<std::ifstream> > stream = provider->ReadStream(path); stream.
                has_value()) {
                return stream;
            }
        }
    }
    return std::nullopt;
}

std::vector<std::string> glimmer::VirtualFileSystem::ListFile(const std::string &path) const {
    LogCat::i("VirtualFileSystem ListFile =", path);
    for (auto &provider: fileProviders_) {
        if (provider->Exists(path) && provider->IsFile(path) == false) {
            auto content = provider->ListFile(path);
            unsigned long count = content.size();
            if (count > 0) {
                return content;
            }
        }
    }
    return {};
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
