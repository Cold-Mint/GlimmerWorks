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
#include "nlohmann/json.hpp"

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
    std::set<std::string> resultSet;
    bool find = false;
    std::string prefix = path;
    if (!prefix.empty() && prefix.back() != '/') {
        prefix += '/';
    }

    for (const auto &item : assetIndexData) {
        if (item.path == path) {
            find = true;
            continue;
        }

        if (item.path.rfind(prefix, 0) != 0) {
            continue;
        }

        std::string remainder = item.path.substr(prefix.size());
        size_t pos = remainder.find('/');
        std::string firstLevel = (pos == std::string::npos) ? remainder : remainder.substr(0, pos);

        if (!firstLevel.empty()) {
            resultSet.insert(prefix + firstLevel);
        }
    }

    if (find) {
        return std::vector<std::string>(resultSet.begin(), resultSet.end());
    }

    return {};
}

    std::optional<std::string>
    AndroidAssetsFileProvider::GetActualPath(const std::string &path) const {
        return std::nullopt;
    }

    bool AndroidAssetsFileProvider::CreateFolder(const std::string &path) {
        return false;
    }

    bool AndroidAssetsFileProvider::SetIndex(nlohmann::json json) {
        assetIndexData.clear();

        // JSON 基础字段检查
        if (!json.contains("assets") || !json["assets"].is_array()) {
            // JSON 不合法
            return false;
        }

        try {
            for (const auto &item: json["assets"]) {
                if (!item.contains("path") || !item.contains("isFile"))
                    continue;

                if (!item["path"].is_string() || !item["isFile"].is_boolean())
                    continue;

                AssetIndex index;
                index.path = item["path"].get<std::string>();
                index.isFile = item["isFile"].get<bool>();
                LogCat::d("AndroidAssetsFileProvider Add", index.path);
                assetIndexData.emplace_back(std::move(index));
            }
        } catch (...) {
            return false;
        }
        return true;
    }
}
#endif
