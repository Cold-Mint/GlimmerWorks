//
// Created by coldmint on 2025/11/28.
//

#ifndef GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H
#define GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H

#ifdef __ANDROID__
#include "FileProvider.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "nlohmann/json_fwd.hpp"

namespace glimmer {
    struct AssetIndex {
        std::string path;
        bool isFile;
    };

    class AndroidAssetsFileProvider : public FileProvider {
        AAssetManager *assetManager_{};

        std::vector<AssetIndex> assetIndexData = {};

    public:
        explicit AndroidAssetsFileProvider(AAssetManager *assetManager);


        [[nodiscard]] bool SetIndex(nlohmann::json json);

        [[nodiscard]] std::string GetFileProviderName() const override;

        [[nodiscard]] std::optional<std::string> ReadFile(const std::string &path) override;

        [[nodiscard]] std::optional<std::unique_ptr<std::istream> >
        ReadStream(const std::string &path) override;

        [[nodiscard]] bool Exists(const std::string &path) override;

        [[nodiscard]] bool IsFile(const std::string &path) override;

        [[nodiscard]] bool WriteFile(const std::string &path, const std::string &content) override;

        [[nodiscard]] std::vector<std::string> ListFile(const std::string &path) override;

        [[nodiscard]] std::optional<std::string>
        GetActualPath(const std::string &path) const override;

        bool CreateFolder(const std::string &path) override;
    };
}
#endif
#endif //GLIMMERWORKS_ANDROIDASSETSFILEPROVIDER_H
