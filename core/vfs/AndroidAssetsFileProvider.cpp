//
// Created by Cold-Mint on 2025/11/28.
//

#ifdef __ANDROID__
#include <string>
#include "AndroidAssetsFileProvider.h"

#include <filesystem>

#include "../log/LogCat.h"
#include <istream>
#include <set>

#include "AAssetStream.h"
#include "AAssetStreamBuf.h"

namespace glimmer {
    AndroidAssetsFileProvider::AndroidAssetsFileProvider(AAssetManager *assetManager)
        : assetManager_(assetManager) {
    }


    void AndroidAssetsFileProvider::SetAssetEntryData(const std::vector<AndroidAssetEntry> &assetEntryData) {
        assetEntryData_ = assetEntryData;
    }

    std::string AndroidAssetsFileProvider::GetFileProviderName() const {
        return "AndroidAssetsFileProvider";
    }

    std::optional<std::string> AndroidAssetsFileProvider::ReadFile(const std::string &path) {
        AAsset *asset = AAssetManager_open(assetManager_, path.c_str(), AASSET_MODE_UNKNOWN);
        if (!asset) return std::nullopt;

        AAssetStreamBuf buf(asset);
        std::istream is(&buf);
        std::string content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

        return content;
    }

    std::optional<std::unique_ptr<std::istream> >
    AndroidAssetsFileProvider::ReadStream(const std::string &path) {
        AAsset *asset = AAssetManager_open(assetManager_, path.c_str(), AASSET_MODE_UNKNOWN);
        if (!asset) {
            return std::nullopt;
        }

        return std::make_unique<AAssetStream>(asset);
    }

    bool AndroidAssetsFileProvider::Exists(const std::string &path) {
        for (const auto &item: assetEntryData_) {
            if (item.path == path) {
                return true;
            }
        }
        return false;
    }

    bool AndroidAssetsFileProvider::IsFile(const std::string &path) {
        for (const auto &item: assetEntryData_) {
            if (item.path == path) {
                return item.isFile;
            }
        }
        return false;
    }

    bool AndroidAssetsFileProvider::WriteFile(const std::string &path, const std::string &content) {
        return false;
    }


    std::optional<std::string> AndroidAssetsFileProvider::GetFileOrFolderName(const std::string &path) const {
        for (const auto &item: assetEntryData_) {
            if (item.path == path) {
                return std::filesystem::path(path).filename().string();
            }
        }
        return std::nullopt;
    }

    bool AndroidAssetsFileProvider::DeleteFileOrFolder(const std::string &path) {
        return false;
    }

    std::vector<std::string> AndroidAssetsFileProvider::ListFile(const std::string &path, bool recursive) {
        std::vector<std::string> result = {};
        for (const auto &item: assetEntryData_) {
            if (item.path.starts_with(path)) {
                result.push_back(item.path);
            }
        }
        return result;
    }

    std::optional<std::string>
    AndroidAssetsFileProvider::GetActualPath(const std::string &path) const {
        return std::nullopt;
    }

    bool AndroidAssetsFileProvider::CreateFolder(const std::string &path) {
        return false;
    }
}
#endif
