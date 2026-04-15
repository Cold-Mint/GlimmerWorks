//@genCode
//
// Created by Cold-Mint on 2025/11/28.
//

#ifndef GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H
#define GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H

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
#endif //GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H
