//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPack.h"

#include "../../Constants.h"
#include <nlohmann/json.hpp>

#include "StringManager.h"
#include "TileManager.h"
#include "../../log/LogCat.h"
#include "../../utils/JsonUtils.h"
#include "../../core/mod/PackManifest.h"

int glimmer::DataPack::LoadStringResource(const std::string &language, StringManager &stringManager) const {
    const std::string langDir = path_ + "/langs";
    const std::string langFile = langDir + "/" + language + ".json";
    const std::string defaultFile = langDir + "/default.json";

    if (virtualFileSystem_->Exists(langFile)) {
        LogCat::d("Loading language file: ", langFile);
        return LoadStringResourceFromFile(langFile, stringManager);
    }
    if (virtualFileSystem_->Exists(defaultFile)) {
        LogCat::d("Language file not found for ", language, ", using default.json");
        return LoadStringResourceFromFile(defaultFile, stringManager);
    }
    LogCat::w("No language file found in ", langDir);
    return 0;
}

int glimmer::DataPack::LoadTileResource(TileManager &tileManager) const {
    const std::string tilesDir = path_ + "/tiles";
    if (!virtualFileSystem_->Exists(tilesDir)) {
        LogCat::w("No tiles directory found in ", tilesDir);
        return 0;
    }
    std::vector<std::string> files = virtualFileSystem_->ListFile(tilesDir);
    if (files.empty()) {
        LogCat::w("No tiles files found in ", tilesDir);
        return 0;
    }
    int tileCount = 0;
    for (const auto &file: files) {
        LogCat::d("Loading tile file: ", file);
        if (LoadTileResourceFromFile(file, tileManager)) {
            tileCount++;
        }
    }
    return tileCount;
}

int glimmer::DataPack::LoadBiomeResource(BiomesManager &biomesManager) const {
    const std::string biomesDir = path_ + "/biomes";
    if (!virtualFileSystem_->Exists(biomesDir)) {
        LogCat::w("No biomes directory found in ", biomesDir);
        return 0;
    }
    std::vector<std::string> files = virtualFileSystem_->ListFile(biomesDir);
    if (files.empty()) {
        LogCat::w("No biomes files found in ", biomesDir);
        return 0;
    }
    int biomeCount = 0;
    for (const auto &file: files) {
        LogCat::d("Loading biomes file: ", file);
        if (LoadBiomeResourceFromFile(file, biomesManager)) {
            biomeCount++;
        }
    }
    return biomeCount;
}

int glimmer::DataPack::LoadStringResourceFromFile(const std::string &path, StringManager &stringManager) const {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(virtualFileSystem_, path);
    if (!jsonOpt) {
        LogCat::e("Failed to load JSON file: ", path);
        return 0;
    }

    const auto &jsonObject = *jsonOpt;
    if (!jsonObject.is_array()) {
        LogCat::e("Invalid language file format (expected array): ", path);
        return 0;
    }

    int count = 0;
    for (const auto &item: jsonObject) {
        auto stringRes = item.get<StringResource>();
        stringRes.packId = manifest_.id;
        stringManager.RegisterResource(stringRes);
        count++;
    }

    LogCat::i("Loaded ", count, " language entries from ", path);
    return count;
}

bool glimmer::DataPack::LoadTileResourceFromFile(const std::string &path, TileManager &tileManager) const {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(virtualFileSystem_, path);
    if (!jsonOpt) {
        LogCat::e("Failed to load JSON file: ", path);
        return false;
    }

    const auto &jsonObject = *jsonOpt;
    TileResource tileResource = jsonObject.get<TileResource>();
    tileResource.packId = manifest_.id;
    tileManager.RegisterResource(tileResource);
    return true;
}

bool glimmer::DataPack::LoadBiomeResourceFromFile(const std::string &path, BiomesManager &biomesManager) const {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(virtualFileSystem_, path);
    if (!jsonOpt) {
        LogCat::e("Failed to load JSON file: ", path);
        return false;
    }

    const auto &jsonObject = *jsonOpt;
    BiomeResource biomeResource = jsonObject.get<BiomeResource>();
    biomeResource.packId = manifest_.id;
    biomesManager.RegisterResource(biomeResource);
    return true;
}

bool glimmer::DataPack::LoadManifest() {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(virtualFileSystem_, path_ + "/" + MANIFEST_FILE_NAME);
    if (!jsonOpt) {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path_ + "/" + MANIFEST_FILE_NAME);
        return false;
    }

    const auto &jsonObject = *jsonOpt;

    try {
        manifest_ = jsonObject.get<DataPackManifest>();
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest JSON: ", e.what());
        return false;
    }
    LogCat::d("DataPack::loadManifest - Loaded manifest for data pack: ", path_);
    LogCat::d("ID: ", manifest_.id);
    LogCat::d("Name: ", manifest_.name.resourceKey, " (packId: ", manifest_.name.packId, ")");
    LogCat::d("Description: ", manifest_.description.resourceKey, " (packId: ", manifest_.description.packId, ")");
    LogCat::d("Author: ", manifest_.author);
    LogCat::d("Version: ", manifest_.versionName, " (Number: ", manifest_.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest_.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest_.resPack ? "true" : "false");
    return true;
}

bool glimmer::DataPack::LoadPack(const std::string &language, StringManager &stringManager, TileManager &tileManager,
                                 BiomesManager &biomesManager) const {
    int total = 0;
    total += LoadStringResource(language, stringManager);
    total += LoadTileResource(tileManager);
    total += LoadBiomeResource(biomesManager);
    return total != 0;
}


const glimmer::DataPackManifest &glimmer::DataPack::GetManifest() const {
    return manifest_;
}
