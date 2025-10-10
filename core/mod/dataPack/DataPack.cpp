//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPack.h"

#include "../../Constants.h"
#include <nlohmann/json.hpp>

#include "../../log/LogCat.h"
#include "../../utils/JsonUtils.h"

bool Glimmer::DataPack::loadManifest() {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(path + "/" + MANIFEST_FILE_NAME);
    if (!jsonOpt) {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path + "/" + MANIFEST_FILE_NAME);
        return false;
    }

    const auto &jsonObject = *jsonOpt;

    try {
        manifest = jsonObject.get<PackManifest>();
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest JSON: ", e.what());
        return false;
    }
    LogCat::d("DataPack::loadManifest - Loaded manifest for data pack: ", path);
    LogCat::d("ID: ", manifest.id);
    LogCat::d("Name: ", manifest.name.key, " (packId: ", manifest.name.packId, ")");
    LogCat::d("Description: ", manifest.description.key, " (packId: ", manifest.description.packId, ")");
    LogCat::d("Author: ", manifest.author);
    LogCat::d("Version: ", manifest.versionName, " (Number: ", manifest.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest.resPack ? "true" : "false");
    return true;
}

const Glimmer::PackManifest & Glimmer::DataPack::getManifest() const {
    return manifest;
}

