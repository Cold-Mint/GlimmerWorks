//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPackManager.h"

#include <filesystem>
#include <regex>
#include "DataPack.h"
#include "../../log/LogCat.h"
#include "../../Config.h"

namespace Glimmer {
    class Config;
}

namespace fs = std::filesystem;

bool Glimmer::DataPackManager::isDataPackAvailable(const DataPack &pack) {
    const PackManifest &manifest = pack.getManifest();
    static const std::regex uuidPattern(
        "^[0-9a-fA-F]{8}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{4}-"
        "[0-9a-fA-F]{12}$"
    );

    if (!std::regex_match(manifest.id, uuidPattern)) {
        LogCat::e("Invalid DataPack id format: ", manifest.id);
        return false;
    }
    if (manifest.minGameVersion > GAME_VERSION_NUMBER) {
        LogCat::e("DataPack ", manifest.id, " requires game version ",
                  manifest.minGameVersion, ", current version: ", GAME_VERSION_NUMBER);
        return false;
    }
    return true;
}

bool Glimmer::DataPackManager::isDataPackEnabled(const DataPack &pack, const Config &config) {
    const auto &enabledList = config.mods.enabledDataPack;
    return std::ranges::find(enabledList, pack.getManifest().id) != enabledList.end();
}

int Glimmer::DataPackManager::scan(std::string &path) {
    try {
        if (!fs::exists(path)) {
            LogCat::e("DataPackManager: Path does not exist -> ", path);
            return 0;
        }

        LogCat::i("Scanning data packs in: ", path);
        const Config &config = Config::getInstance();

        int success = 0;
        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                const std::string folderPath = entry.path().string();
                LogCat::d("Found data pack folder: ", folderPath);
                DataPack pack(folderPath);
                if (!pack.loadManifest()) {
                    continue;
                }
                // Determine whether the data packet is enabled
                // 判断数据包是否启用
                if (!isDataPackEnabled(pack, config)) {
                    LogCat::w("Data pack not enabled: ", pack.getManifest().id);
                    continue;
                }
                if (!isDataPackAvailable(pack)) {
                    continue;
                }
                if (pack.loadPack()) {
                    success++;
                }
            }
        }
        return success;
    } catch (const std::exception &e) {
        LogCat::e("DataPackManager::scan failed: ", e.what());
        return 0;
    }
}
