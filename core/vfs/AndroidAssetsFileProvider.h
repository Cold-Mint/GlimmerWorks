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
#pragma once
//@genCode
#ifdef __ANDROID__
#include <algorithm>
#include <string>
#include "FileProvider.h"
#include <android/asset_manager.h>

#include "toml11/find.hpp"
#include "toml11/types.hpp"

namespace glimmer {
    struct AndroidAssetEntry {
        std::string path;
        std::string sha256;
        bool isFile = false;
    };
}

namespace toml {
    template<>
    struct from<glimmer::AndroidAssetEntry> {
        static glimmer::AndroidAssetEntry from_toml(const value &v) {
            glimmer::AndroidAssetEntry r;
            r.path = toml::find<std::string>(v, "path");
            r.isFile = toml::find<bool>(v, "is_file");
            r.isFile = toml::find<bool>(v, "is_file");
            return r;
        }
    };
}

namespace glimmer {
    class AndroidAssetsFileProvider : public IFileProvider {
        AAssetManager *assetManager_{};

        std::vector<AndroidAssetEntry> assetEntryData_ = {};

    public:
        explicit AndroidAssetsFileProvider(AAssetManager *assetManager);


        void SetAssetEntryData(const std::vector<AndroidAssetEntry> &assetEntryData);

        [[nodiscard]] std::string GetFileProviderName() const override;

        [[nodiscard]] std::optional<std::string> ReadFile(const std::string &path) override;

        [[nodiscard]] std::optional<std::unique_ptr<std::istream> >
        ReadStream(const std::string &path) override;

        [[nodiscard]] bool Exists(const std::string &path) override;

        [[nodiscard]] bool IsFile(const std::string &path) override;

        [[nodiscard]] bool WriteFile(const std::string &path, const std::string &content) override;

        [[nodiscard]] std::optional<std::string> GetFileOrFolderName(const std::string &path) const override;

        [[nodiscard]] bool DeleteFileOrFolder(const std::string &path) override;

        [[nodiscard]] std::vector<std::string> ListFile(const std::string &path, bool recursive) override;

        [[nodiscard]] std::optional<std::string>
        GetActualPath(const std::string &path) const override;

        bool CreateFolder(const std::string &path) override;
    };
}

#endif
