//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPack.h"

#include "../../Constants.h"
#include <nlohmann/json.hpp>

#include "StringManager.h"
#include "../../log/LogCat.h"
#include "../../utils/JsonUtils.h"
#include "../../core/mod/PackManifest.h"
namespace fs = std::filesystem;

int glimmer::DataPack::LoadStringResource(const std::string &language, StringManager &stringManager) const {
    const fs::path langDir = fs::path(path) / "langs";
    const fs::path langFile = langDir / (language + ".json");
    const fs::path defaultFile = langDir / "default.json";
    if (exists(langFile)) {
        LogCat::d("Loading language file: ", langFile.string());
        return LoadStringResourceFromFile(langFile.string(), stringManager);
    }
    if (exists(defaultFile)) {
        LogCat::d("Language file not found for ", language, ", using default.json");
        return LoadStringResourceFromFile(defaultFile.string(), stringManager);
    }
    LogCat::w("No language file found in ", langDir.string());
    return 0;
}

int glimmer::DataPack::LoadStringResourceFromFile(const std::string &path, StringManager &stringManager) const {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(path);
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
        stringRes.packId = manifest.id;
        stringManager.RegisterResource(stringRes);
        count++;
    }

    LogCat::i("Loaded ", count, " language entries from ", path);
    return count;
}

bool glimmer::DataPack::LoadManifest() {
    const auto jsonOpt = JsonUtils::LoadJsonFromFile(path + "/" + MANIFEST_FILE_NAME);
    if (!jsonOpt) {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path + "/" + MANIFEST_FILE_NAME);
        return false;
    }

    const auto &jsonObject = *jsonOpt;

    try {
        manifest = jsonObject.get<DataPackManifest>();
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest JSON: ", e.what());
        return false;
    }
    LogCat::d("DataPack::loadManifest - Loaded manifest for data pack: ", path);
    LogCat::d("ID: ", manifest.id);
    LogCat::d("Name: ", manifest.name.resourceKey, " (packId: ", manifest.name.packId, ")");
    LogCat::d("Description: ", manifest.description.resourceKey, " (packId: ", manifest.description.packId, ")");
    LogCat::d("Author: ", manifest.author);
    LogCat::d("Version: ", manifest.versionName, " (Number: ", manifest.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest.resPack ? "true" : "false");
    return true;
}

bool glimmer::DataPack::LoadPack(const std::string &language, StringManager &stringManager) const {
    return LoadStringResource(language, stringManager) != 0;
}

const glimmer::DataPackManifest &glimmer::DataPack::GetManifest() const {
    return manifest;
}
