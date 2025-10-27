//
// Created by Cold-Mint on 2025/10/15.
//

#include "ResourcePack.h"

#include "../../Constants.h"
#include "../../log/LogCat.h"
#include "../../utils/JsonUtils.h"

bool glimmer::ResourcePack::loadManifest() {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(path + "/" + MANIFEST_FILE_NAME);
    if (!jsonOpt) {
        LogCat::e("ResourcePack::loadManifest - Failed to load manifest: ", path + "/" + MANIFEST_FILE_NAME);
        return false;
    }

    const auto &jsonObject = *jsonOpt;

    try {
        manifest = jsonObject.get<ResourcePackManifest>();
    } catch (const std::exception &e) {
        LogCat::e("ResourcePack::loadManifest - Failed to parse manifest JSON: ", e.what());
        return false;
    }
    LogCat::d("ResourcePack::loadManifest - Loaded manifest for data pack: ", path);
    LogCat::d("ID: ", manifest.id);
    LogCat::d("Name: ", manifest.name.resourceKey, " (packId: ", manifest.name.packId, ")");
    LogCat::d("Description: ", manifest.description.resourceKey, " (packId: ", manifest.description.packId, ")");
    LogCat::d("Author: ", manifest.author);
    LogCat::d("Version: ", manifest.versionName, " (Number: ", manifest.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest.resPack ? "true" : "false");
    return true;
}


const glimmer::ResourcePackManifest &glimmer::ResourcePack::getManifest() const {
    return manifest;
}

std::string_view glimmer::ResourcePack::getPath() const {
    return path;
}
