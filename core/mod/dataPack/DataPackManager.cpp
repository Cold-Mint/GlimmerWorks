//
// Created by Cold-Mint on 2025/10/10.
//
#include "DataPackManager.h"

#include <regex>
#include <algorithm>
#include "DataPack.h"
#include "../../log/LogCat.h"
#include "core/scene/AppContext.h"
#include "toml11/spec.hpp"

namespace glimmer {
    class Config;
}

namespace fs = std::filesystem;

bool glimmer::DataPackManager::IsDataPackAvailable(const DataPack &pack) const {
    const PackManifest &manifest = pack.GetManifest();
    if (manifest.id == RESOURCE_REF_CORE || manifest.id == RESOURCE_REF_SELF) {
        return false;
    }
    for (const auto &packId: packManifestVector_) {
        if (packId.id == manifest.id) {
            LogCat::i("Duplicate package ID: ", packId.id);
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

glimmer::DataPackManager::DataPackManager(VirtualFileSystem *virtualFilesystem) : virtualFileSystem_(
    virtualFilesystem) {
}

bool glimmer::DataPackManager::IsDependencySatisfied(const std::string &pack1Id, const std::string &pack2Id) {
    if (pack1Id == pack2Id) {
        return true;
    }
    const DataPackManifest *dataPackManifest1 = nullptr;
    for (auto &packId: packManifestVector_) {
        if (packId.id == pack1Id) {
            dataPackManifest1 = &packId;
            break;
        }
    }
    if (dataPackManifest1 == nullptr) {
        return false;
    }
    if (pack2Id == RESOURCE_REF_CORE) {
        bool match = false;
        for (auto &packDependency: dataPackManifest1->packDependencies) {
            if (packDependency.packId == pack2Id) {
                //If the dependencies within data packet 1 include the ID of data packet 2.
                //如果数据包1内的依赖项包含数据包2的id。
                if (CORE_DATA_PACK_VERSION_NUMBER >= packDependency.minVersion) {
                    match = true;
                    break;
                }
            }
        }
        return match;
    }
    DataPackManifest *dataPackManifest2 = nullptr;
    for (auto &packId: packManifestVector_) {
        if (packId.id == pack2Id) {
            dataPackManifest2 = &packId;
            break;
        }
    }
    if (dataPackManifest2 == nullptr) {
        return false;
    }
    bool match = false;
    for (auto &packDependency: dataPackManifest1->packDependencies) {
        if (packDependency.packId == pack2Id) {
            //If the dependencies within data packet 1 include the ID of data packet 2.
            //如果数据包1内的依赖项包含数据包2的id。
            if (dataPackManifest2->versionNumber >= packDependency.minVersion) {
                //If the dependency version in package 1 is less than or equal to the version of package 2.
                //如果包1内的依赖版本小于或等于数据包2的版本。
                match = true;
                break;
            }
        }
    }
    return match;
}

int glimmer::DataPackManager::Scan(AppContext *appContext, const toml::spec &tomlVersion) {
    const std::string &path = appContext->GetConfig()->mods.dataPackPath;
    if (!virtualFileSystem_->Exists(path)) {
        LogCat::e("DataPackManager: Path does not exist -> ", path);
        return 0;
    }
    packManifestVector_.clear();
    LogCat::i("Scanning data packs in: ", path);
    int success = 0;
    std::vector<std::string> files = virtualFileSystem_->ListFile(path, false);
    for (const auto &entry: files) {
        if (!virtualFileSystem_->IsFile(entry)) {
            LogCat::d("Found data pack folder: ", entry);
            DataPack pack(entry, virtualFileSystem_, tomlVersion);
            if (!pack.LoadManifest()) {
                continue;
            }
            // Determine whether the data packet is enabled
            // 判断数据包是否启用
            if (!IsDataPackEnabled(pack, appContext->GetConfig()->mods.enabledDataPack)) {
                LogCat::w("Data pack not enabled: ", pack.GetManifest().id);
                continue;
            }
            if (!IsDataPackAvailable(pack)) {
                continue;
            }
            if (pack.LoadPack(appContext)) {
                success++;
                packManifestVector_.push_back(pack.GetManifest());
            }
        }
    }
    return success;
}
