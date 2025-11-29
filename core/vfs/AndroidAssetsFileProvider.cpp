//
// Created by coldmint on 2025/11/28.
//

#include "AndroidAssetsFileProvider.h"
#include "../log/LogCat.h"
#include <istream>

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
        std::vector<std::string> result = {};
        bool find = false;
        for (const auto &item: assetIndexData) {
            if (item.path == path) {
                find = true;
                continue;
            }
            // 判断前缀
            if (item.path.size() >= path.size() && item.path.compare(0, path.size(), path) == 0) {
                // 确保下一个字符是 '/'，避免 "mods" 匹配 "mods2"
                if (item.path.size() > path.size() &&
                    (item.path[path.size()] == '/' || item.path[path.size()] == '\\')) {
                    result.push_back(item.path);
                }
            }
        }
        if (find) {
            return result;
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
