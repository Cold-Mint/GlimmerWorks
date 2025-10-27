//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPackManager.h"

#include <filesystem>
#include <regex>
#include "DataPack.h"
#include "../../log/LogCat.h"

namespace glimmer {
    class Config;
}

namespace fs = std::filesystem;

bool glimmer::DataPackManager::IsDataPackAvailable(const DataPack &pack) {
    const PackManifest &manifest = pack.GetManifest();
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
    return !manifest.resPack;
}

bool glimmer::DataPackManager::IsDataPackEnabled(const DataPack &pack,
                                                 const std::vector<std::string> &enabledDataPack) {
    return std::ranges::find(enabledDataPack, pack.GetManifest().id) != enabledDataPack.end();
}

int glimmer::DataPackManager::Scan(const std::string &path, const std::vector<std::string> &enabledDataPack,
                                   const std::string &language,  StringManager & stringManager) {
    try {
        if (!fs::exists(path)) {
            LogCat::e("DataPackManager: Path does not exist -> ", path);
            return 0;
        }

        LogCat::i("Scanning data packs in: ", path);
        int success = 0;
        for (const auto &entry: fs::directory_iterator(path)) {
            if (entry.is_directory()) {
                const std::string folderPath = entry.path().string();
                LogCat::d("Found data pack folder: ", folderPath);
                DataPack pack(folderPath);
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
                if (pack.LoadPack(language,stringManager)) {
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
