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
