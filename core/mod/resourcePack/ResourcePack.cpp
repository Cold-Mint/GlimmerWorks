//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePack.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../utils/TomlUtils.h"
#include "toml11/parser.hpp"


glimmer::ResourcePack::ResourcePack(std::string path, const VirtualFileSystem *virtualFileSystem,
                                    const toml::spec &tomlVersion) : path_(std::move(path)),
                                                                     virtualFileSystem_(virtualFileSystem), manifest_(),
                                                                     tomlVersion_(tomlVersion) {
}

bool glimmer::ResourcePack::loadManifest() {
    auto data =
            virtualFileSystem_->ReadFile(path_ + "/" + MANIFEST_FILE_NAME);
    if (!data.has_value()) {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path_ + "/" + MANIFEST_FILE_NAME);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    try {
        manifest_ = toml::get<ResourcePackManifest>(value);
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest toml: ", e.what());
        return false;
    }
    manifest_.name.SetSelfPackageId(manifest_.id);
    manifest_.description.SetSelfPackageId(manifest_.id);
    LogCat::d("ResourcePack::loadManifest - Loaded manifest for data pack: ", path_);
    LogCat::d("ID: ", manifest_.id);
    LogCat::d("Name: ", manifest_.name.GetResourceKey(), " (packId: ", manifest_.name.GetPackageId(), ")");
    LogCat::d("Description: ", manifest_.description.GetResourceKey(), " (packId: ",
              manifest_.description.GetPackageId(),
              ")");
    LogCat::d("Author: ", manifest_.author);
    LogCat::d("Version: ", manifest_.versionName, " (Number: ", manifest_.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest_.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest_.resPack ? "true" : "false");
    return true;
}

const glimmer::ResourcePackManifest &glimmer::ResourcePack::getManifest() const {
    return manifest_;
}

std::string glimmer::ResourcePack::getPath() const {
    return path_;
}
