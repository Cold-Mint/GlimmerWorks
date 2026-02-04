//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPack.h"

#include "../../Constants.h"

#include "StringManager.h"
#include "TileManager.h"
#include "../../log/LogCat.h"
#include "../../utils/TomlUtils.h"
#include "../../core/mod/PackManifest.h"
#include "core/lootTable/LootTableManager.h"
#include "toml11/parser.hpp"

int glimmer::DataPack::LoadStringResource(const std::string &language, StringManager *stringManager) const {
    const std::string langDir = path_ + "/langs";
    const std::string langFile = langDir + "/" + language + ".toml";
    const std::string defaultFile = langDir + "/default.toml";

    if (virtualFileSystem_->Exists(langFile)) {
        LogCat::d("Loading language file: ", langFile);
        return LoadStringResourceFromFile(langFile, stringManager);
    }
    if (virtualFileSystem_->Exists(defaultFile)) {
        LogCat::d("Language file not found for ", language, ", using default.toml");
        return LoadStringResourceFromFile(defaultFile, stringManager);
    }
    LogCat::w("No language file found in ", langDir);
    return 0;
}

int glimmer::DataPack::LoadTileResource(TileManager *tileManager) const {
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

int glimmer::DataPack::LoadBiomeResource(BiomesManager *biomesManager) const {
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

int glimmer::DataPack::LoadComposableItemResource(ItemManager *itemManager) const {
    const std::string itemsDir = path_ + "/composableItems";
    if (!virtualFileSystem_->Exists(itemsDir)) {
        LogCat::w("No items directory found in ", itemsDir);
        return 0;
    }
    std::vector<std::string> files = virtualFileSystem_->ListFile(itemsDir);
    if (files.empty()) {
        LogCat::w("No items files found in ", itemsDir);
        return 0;
    }
    int itemCount = 0;
    for (const auto &file: files) {
        LogCat::d("Loading composableItem file: ", file);
        if (LoadComposableItemResourceFromFile(file, itemManager)) {
            itemCount++;
        }
    }
    return itemCount;
}

int glimmer::DataPack::LoadAbilityItemResource(ItemManager *itemManager) const {
    const std::string itemsDir = path_ + "/abilityItems";
    if (!virtualFileSystem_->Exists(itemsDir)) {
        LogCat::w("No items directory found in ", itemsDir);
        return 0;
    }
    std::vector<std::string> files = virtualFileSystem_->ListFile(itemsDir);
    if (files.empty()) {
        LogCat::w("No items files found in ", itemsDir);
        return 0;
    }
    int itemCount = 0;
    for (const auto &file: files) {
        LogCat::d("Loading abilityItem file: ", file);
        if (LoadAbilityItemResourceFromFile(file, itemManager)) {
            itemCount++;
        }
    }
    return itemCount;
}

int glimmer::DataPack::LoadLootTableResource(LootTableManager *lootTableManager) const {
    const std::string itemsDir = path_ + "/lootTables";
    if (!virtualFileSystem_->Exists(itemsDir)) {
        LogCat::w("No items directory found in ", itemsDir);
        return 0;
    }
    std::vector<std::string> files = virtualFileSystem_->ListFile(itemsDir);
    if (files.empty()) {
        LogCat::w("No items files found in ", itemsDir);
        return 0;
    }
    int itemCount = 0;
    for (const auto &file: files) {
        LogCat::d("Loading lootTable file: ", file);
        if (LoadLootTableResourceFromFile(file, lootTableManager)) {
            itemCount++;
        }
    }
    return itemCount;
}


int glimmer::DataPack::LoadStringResourceFromFile(const std::string &path, StringManager *stringManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    try {
        int count = 0;
        auto array = toml::find<std::vector<StringResource> >(value, "string");
        for (auto &stringRes: array) {
            stringRes.packId = manifest_.id;
            stringManager->AddResource(
                std::make_unique<StringResource>(std::move(stringRes))
            );
            count++;
        }
        LogCat::i("Loaded ", count, " language entries from ", path);
        return count;
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest toml: ", e.what());
        return false;
    }
}

bool glimmer::DataPack::
LoadLootTableResourceFromFile(const std::string &path, LootTableManager *lootTableManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto lootResource = std::make_unique<LootResource>(toml::get<LootResource>(value));
    lootResource->packId = manifest_.id;
    for (auto &mandatory: lootResource->mandatory) {
        mandatory.item.SetSelfPackageId(manifest_.id);
        mandatory.mandatory = true;
    }
    for (auto &pool: lootResource->pool) {
        pool.item.SetSelfPackageId(manifest_.id);
        pool.mandatory = false;
    }
    lootTableManager->AddResource(std::move(lootResource));
    return true;
}


bool glimmer::DataPack::LoadTileResourceFromFile(const std::string &path, TileManager *tileManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto tileResource = std::make_unique<TileResource>(toml::get<TileResource>(value));
    tileResource->packId = manifest_.id;
    tileResource->name.SetSelfPackageId(manifest_.id);
    tileResource->description.SetSelfPackageId(manifest_.id);
    if (tileResource->customLootTable) {
        tileResource->lootTable.SetSelfPackageId(manifest_.id);
    }
    tileManager->AddResource(std::move(tileResource));
    return true;
}

bool glimmer::DataPack::LoadBiomeResourceFromFile(const std::string &path, BiomesManager *biomesManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto biomeResource = std::make_unique<BiomeResource>(toml::get<BiomeResource>(value));
    biomeResource->packId = manifest_.id;
    biomeResource->baseTileRef.SetSelfPackageId(manifest_.id);
    biomesManager->AddResource(std::move(biomeResource));
    return true;
}

bool glimmer::DataPack::LoadComposableItemResourceFromFile(const std::string &path, ItemManager *itemManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    const toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto itemResource = std::make_unique<ComposableItemResource>(toml::get<ComposableItemResource>(value));
    itemResource->packId = manifest_.id;
    itemResource->name.SetSelfPackageId(manifest_.id);
    itemResource->description.SetSelfPackageId(manifest_.id);
    for (auto &defaultAbility: itemResource->defaultAbilityList) {
        defaultAbility.SetSelfPackageId(manifest_.id);
    }
    itemManager->AddComposableResource(std::move(itemResource));
    return true;
}

bool glimmer::DataPack::LoadAbilityItemResourceFromFile(const std::string &path, ItemManager *itemManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto itemResource = std::make_unique<AbilityItemResource>(toml::get<AbilityItemResource>(value));
    itemResource->packId = manifest_.id;
    itemResource->name.SetSelfPackageId(manifest_.id);
    itemResource->description.SetSelfPackageId(manifest_.id);
    itemManager->AddAbilityItemResource(std::move(itemResource));
    return true;
}

glimmer::DataPack::DataPack(std::string path, const VirtualFileSystem *virtualFileSystem,
                            const toml::spec &tomlVersion) : path_(std::move(path)),
                                                             manifest_(), tomlVersion_(tomlVersion),
                                                             virtualFileSystem_(virtualFileSystem) {
}

bool glimmer::DataPack::LoadManifest() {
    auto data =
            virtualFileSystem_->ReadFile(path_ + "/" + MANIFEST_FILE_NAME);
    if (!data.has_value()) {
        LogCat::e("DataPack::loadManifest - Failed to load manifest: ", path_ + "/" + MANIFEST_FILE_NAME);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    try {
        manifest_ = toml::get<DataPackManifest>(value);
    } catch (const std::exception &e) {
        LogCat::e("DataPack::loadManifest - Failed to parse manifest toml: ", e.what());
        return false;
    }
    manifest_.name.SetSelfPackageId(manifest_.id);
    manifest_.description.SetSelfPackageId(manifest_.id);
    LogCat::d("DataPack::loadManifest - Loaded manifest for data pack: ", path_);
    LogCat::d("ID: ", manifest_.id);
    LogCat::d("Name: ", manifest_.name.GetResourceKey(), " (packId: ", manifest_.name.GetPackageId(), ")");
    LogCat::d("Description: ", manifest_.description.GetResourceKey(), " (packId: ",
              manifest_.description.GetPackageId(), ")");
    LogCat::d("Author: ", manifest_.author);
    LogCat::d("Version: ", manifest_.versionName, " (Number: ", manifest_.versionNumber, ")");
    LogCat::d("Minimum Game Version: ", manifest_.minGameVersion);
    LogCat::d("Is Resource Pack: ", manifest_.resPack ? "true" : "false");
    return true;
}

bool glimmer::DataPack::LoadPack(const std::string &language,
                                 StringManager *stringManager, TileManager *tileManager,
                                 BiomesManager *biomesManager, ItemManager *itemManager,
                                 LootTableManager *lootTableManager) const {
    int total = 0;
    total += LoadStringResource(language, stringManager);
    total += LoadTileResource(tileManager);
    total += LoadBiomeResource(biomesManager);
    total += LoadComposableItemResource(itemManager);
    total += LoadAbilityItemResource(itemManager);
    total += LoadLootTableResource(lootTableManager);
    return total != 0;
}


const glimmer::DataPackManifest &glimmer::DataPack::GetManifest() const {
    return manifest_;
}
