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
#include "core/scene/AppContext.h"
#include "toml11/parser.hpp"

std::optional<std::string> glimmer::DataPack::GetDataType(const std::string &path) const {
    auto fileNameOptional = virtualFileSystem_->GetFileOrFolderName(path);
    if (!fileNameOptional.has_value()) {
        return std::nullopt;
    }
    const std::string &fileName = fileNameOptional.value();
    const size_t lastDot = fileName.rfind('.');
    if (lastDot == std::string::npos)
        return std::nullopt;

    const size_t secondLastDot = fileName.rfind('.', lastDot - 1);
    if (secondLastDot == std::string::npos)
        return std::nullopt;

    const std::string format = fileName.substr(lastDot + 1);
    //The TOML format is mandatory.
    //toml格式是强制的。
    if (format != "toml")
        return std::nullopt;
    return fileName.substr(secondLastDot + 1,
                           lastDot - secondLastDot - 1);
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

bool glimmer::DataPack::
LoadStructureResourceFromFile(const std::string &path, StructureManager *structureManager) const {
    auto data =
            virtualFileSystem_->ReadFile(path);
    if (!data.has_value()) {
        LogCat::e("Failed to load toml file: ", path);
        return false;
    }
    toml::value value = toml::parse_str(data.value(), tomlVersion_);
    auto structureResource = std::make_unique<StructureResource>(toml::get<StructureResource>(value));
    structureResource->packId = manifest_.id;
    for (auto &ref: structureResource->data) {
        ref.SetSelfPackageId(manifest_.id);
    }
    structureManager->AddResource(std::move(structureResource));
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
    for (auto &decorator: biomeResource->decorator) {
        for (auto &tile: decorator.data) {
            tile.SetSelfPackageId(manifest_.id);
        }
    }
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

std::optional<std::string> glimmer::DataPack::ExtractLanguageFromFileName(const std::string &fileName) {
    constexpr std::string_view suffix = ".strings.toml";
    if (!fileName.ends_with(suffix)) {
        return std::nullopt;
    }
    std::string base =
            fileName.substr(0, fileName.size() - suffix.size());
    auto pos = base.rfind('.');
    if (pos == std::string::npos) {
        return base;
    }
    return base.substr(pos + 1);
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

bool glimmer::DataPack::LoadPack(AppContext *appContext) const {
    int total = 0;
    std::vector<std::string> files = virtualFileSystem_->ListFile(path_, true);
    if (files.empty()) {
        return false;
    }
    std::vector<std::string> defaultLanguageFiles;
    std::vector<std::string> targetLanguageFiles;
    for (const auto &file: files) {
        const auto dataTypeOptional = GetDataType(file);
        if (!dataTypeOptional.has_value()) {
            continue;
        }
        auto &dataType = dataTypeOptional.value();
        if (dataType == DATA_FILE_TYPE_STRINGS) {
            auto fileNameOptional = virtualFileSystem_->GetFileOrFolderName(file);
            if (!fileNameOptional.has_value()) {
                continue;
            }
            auto &fileName = fileNameOptional.value();
            auto langOptional = ExtractLanguageFromFileName(fileName);
            if (!langOptional.has_value())
                continue;

            const auto &fileLang = langOptional.value();
            if (fileLang == appContext->GetLanguage()) {
                targetLanguageFiles.push_back(file);
            } else if (fileLang == "default") {
                defaultLanguageFiles.push_back(file);
            }
            continue;
        }
        if (dataType == DATA_FILE_TYPE_TILE) {
            LogCat::d("Loading tile file: ", file);
            if (LoadTileResourceFromFile(file, appContext->GetTileManager())) {
                total++;
            }
            continue;
        }
        if (dataType == DATA_FILE_TYPE_BIOME) {
            LogCat::d("Loading biomes file: ", file);
            if (LoadBiomeResourceFromFile(file, appContext->GetBiomesManager())) {
                total++;
            }
            continue;
        }

        if (dataType == DATA_FILE_TYPE_COMPOSABLE_ITEM) {
            LogCat::d("Loading composableItem file: ", file);
            if (LoadComposableItemResourceFromFile(file, appContext->GetItemManager())) {
                total++;
            }
            continue;
        }

        if (dataType == DATA_FILE_TYPE_ABILITY_ITEM) {
            LogCat::d("Loading abilityItem file: ", file);
            if (LoadAbilityItemResourceFromFile(file, appContext->GetItemManager())) {
                total++;
            }
            continue;
        }
        if (dataType == DATA_FILE_TYPE_LOOT_TABLE) {
            LogCat::d("Loading lootTable file: ", file);
            if (LoadLootTableResourceFromFile(file, appContext->GetLootTableManager())) {
                total++;
            }
        }
        if (dataType == DATA_FILE_TYPE_STRUCTURE) {
            LogCat::d("Loading structure file: ", file);
            if (LoadStructureResourceFromFile(file, appContext->GetStructureManager())) {
                total++;
            }
        }
    }
    if (targetLanguageFiles.empty()) {
        for (auto &f: defaultLanguageFiles) {
            total += LoadStringResourceFromFile(f, appContext->GetStringManager());
        }
    } else {
        for (auto &f: targetLanguageFiles) {
            total += LoadStringResourceFromFile(f, appContext->GetStringManager());
        }
    }

    return total != 0;
}


const glimmer::DataPackManifest &glimmer::DataPack::GetManifest() const {
    return manifest_;
}
