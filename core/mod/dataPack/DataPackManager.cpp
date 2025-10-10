//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPackManager.h"

#include <filesystem>

#include "DataPack.h"
#include "../../log/LogCat.h"
#include "../../Config.h"

namespace Glimmer {
    class Config;
}

namespace fs = std::filesystem;

bool Glimmer::DataPackManager::isDataPackAvailable(const DataPack &pack) {
    if (PackManifest manifest = pack.getManifest(); manifest.minGameVersion > GAME_VERSION_NUMBER) {
        LogCat::e("DataPack ", manifest.id, " requires game version ",
                  manifest.minGameVersion, ", current version: ", GAME_VERSION_NUMBER);
        return false;
    }
    return true;
}

int Glimmer::DataPackManager::scan(std::string &path) {
    try {
        if (!fs::exists(path)) {
            LogCat::e("DataPackManager: Path does not exist -> ", path);
            return 0;
        }

        LogCat::i("Scanning data packs in: ", path);
        Config &config = Config::getInstance();
        int success = 0;
        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                const std::string folderPath = entry.path().string();
                LogCat::d("Found data pack folder: ", folderPath);
                DataPack pack(folderPath);
                if (!pack.loadManifest()) {
                    continue;
                }
                if (!isDataPackAvailable(pack)) {
                    continue;
                }
                success++;
            }
        }
        return success;
    } catch (const std::exception &e) {
        LogCat::e("DataPackManager::scan failed: ", e.what());
        return 0;
    }
}
