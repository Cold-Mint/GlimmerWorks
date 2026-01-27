//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPackManager.h"

#include <filesystem>
#include <regex>
#include <algorithm>
#include "DataPack.h"
#include "../../log/LogCat.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class Config;
}

namespace fs = std::filesystem;

bool glimmer::DataPackManager::IsDataPackAvailable(const DataPack &pack) const {
    const PackManifest &manifest = pack.GetManifest();
    for (const auto &packId: packIdVector_) {
        if (packId == manifest.id) {
            LogCat::i("Duplicate package ID: ", packId);
            return false;
        }
    }
    if (manifest.minGameVersion > GAME_VERSION_NUMBER) {
        LogCat::e("DataPack ", manifest.id, " requires game version ",
                  manifest.minGameVersion, ", current version: ", GAME_VERSION_NUMBER);
        return false;
    }
    return !manifest.resPack;
}

bool glimmer::DataPackManager::IsDataPackEnabled(const DataPack &pack,
                                                 const std::vector<std::string> &enabledDataPack) {
    return std::ranges::find(enabledDataPack, pack.GetManifest().id) != enabledDataPack.end();
}

int glimmer::DataPackManager::Scan(const std::string &path, const std::vector<std::string> &enabledDataPack,
                                   const std::string &language, StringManager *stringManager, TileManager *tileManager,
                                   BiomesManager *biomesManager, ItemManager *itemManager,
                                   const toml::spec &tomlVersion) {
    if (!virtualFileSystem_->Exists(path)) {
        LogCat::e("DataPackManager: Path does not exist -> ", path);
        return 0;
    }
    packIdVector_.clear();
    LogCat::i("Scanning data packs in: ", path);
    int success = 0;
    std::vector<std::string> files = virtualFileSystem_->ListFile(path);
    for (const auto &entry: files) {
        if (!virtualFileSystem_->IsFile(entry)) {
            LogCat::d("Found data pack folder: ", entry);
            DataPack pack(entry, virtualFileSystem_, tomlVersion);
            if (!pack.LoadManifest()) {
                continue;
            }
            // Determine whether the data packet is enabled
            // 判断数据包是否启用
            if (!IsDataPackEnabled(pack, enabledDataPack)) {
                LogCat::w("Data pack not enabled: ", pack.GetManifest().id);
                continue;
            }
            if (!IsDataPackAvailable(pack)) {
                continue;
            }
            if (pack.LoadPack(language, stringManager, tileManager, biomesManager, itemManager)) {
                success++;
                packIdVector_.push_back(pack.GetManifest().id);
            }
        }
    }
    return success;
}
