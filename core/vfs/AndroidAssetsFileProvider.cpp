//
// Created by Cold-Mint on 2025/11/28.
//

#ifdef __ANDROID__
#include "AndroidAssetsFileProvider.h"
#include "../log/LogCat.h"
#include <istream>
#include <set>

#include "AAssetStream.h"
#include "AAssetStreamBuf.h"

namespace glimmer {
    AndroidAssetsFileProvider::AndroidAssetsFileProvider(AAssetManager *assetManager)
        : assetManager_(assetManager) {
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
        for (const auto &item: assetIndexData) {
            if (item.path == path) {
                return true;
            }
        }
        return false;
    }

    bool AndroidAssetsFileProvider::IsFile(const std::string &path) {
        for (const auto &item: assetIndexData) {
            if (item.path == path) {
                return item.isFile;
            }
        }
        return false;
    }

    bool AndroidAssetsFileProvider::WriteFile(const std::string &path, const std::string &content) {
        return false;
    }

std::vector<std::string> AndroidAssetsFileProvider::ListFile(const std::string &path) {
    return {};
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
