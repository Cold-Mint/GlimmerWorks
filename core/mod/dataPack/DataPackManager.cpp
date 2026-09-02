/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "DataPackManager.h"

#include "core/context/AppContext.h"

void glimmer::DataPackManager::AfterRegister(DataPack *resource) {
    BasePackManager::AfterRegister(resource);
    packIdVector.emplace_back(resource->GetManifest()->id);
}

void glimmer::DataPackManager::BeforeUnRegister(DataPack *resource) {
    BasePackManager::BeforeUnRegister(resource);
    auto it = std::ranges::find(packIdVector, resource->GetManifest()->id);
    if (it != packIdVector.end()) {
        packIdVector.erase(it);
    }
}

std::unique_ptr<glimmer::DataPack> glimmer::DataPackManager::LoadPack(const PackScanRequest *packScanRequest,
                                                                      std::filesystem::path path) {
    AppContext *appContext = packScanRequest->GetAppContext();
    if (appContext == nullptr) {
        return nullptr;
    }
    VirtualFileSystem *virtualFileSystem = appContext->GetVirtualFileSystem();
    if (virtualFileSystem == nullptr) {
        return nullptr;
    }
    ModContext *modContext = appContext->GetModContext();
    if (modContext == nullptr) {
        return nullptr;
    }

    auto dataPack = std::make_unique<DataPack>(path, virtualFileSystem, modContext->GetTomlTemplateExpander(),
                                               *packScanRequest->GetTomlVersion());
    if (!dataPack->LoadManifest()) {
        LogCat::w(std::source_location::current(), "Failed to load manifest for data pack");
        return nullptr;
    }
    return dataPack;
}

std::filesystem::path glimmer::DataPackManager::GetPackPath(Config *config) const {
    return config->mods.dataPackPath;
}

std::vector<uint64_t> *glimmer::DataPackManager::GetEnabledPack(Config *config) const {
    return &config->mods.enabledDataPack;
}

bool glimmer::DataPackManager::CheckDependencyVersion(const std::vector<PackDependence> &dependencies, uint64_t packId2,
                                                      uint32_t version) {
    return std::ranges::any_of(dependencies, [&](const PackDependence &dep) {
        return dep.packIdUint == packId2 && version >= dep.minVersion;
    });
}


const std::vector<std::string> &glimmer::DataPackManager::GetPackIdVector() const {
    return packIdVector;
}


glimmer::PackVerifyState glimmer::DataPackManager::GetPackVerifyState(uint64_t id) {
    DataPack *dataPack = this->Find(id);
    if (dataPack == nullptr) {
        return PackVerifyState::Unsigned;
    }
    return dataPack->GetPackVerifyState();
}

bool glimmer::DataPackManager::IsDependencySatisfied(uint64_t packId1, uint64_t packId2) {
    if (packId1 == packId2) {
        return true;
    }
    const DataPackManifest *dataPackManifest1 = nullptr;
    DataPack *dataPack1 = this->Find(packId1);
    if (dataPack1 == nullptr) {
        return false;
    }
    dataPackManifest1 = dataPack1->GetManifest();
    if (dataPackManifest1 == nullptr) {
        return false;
    }
    if (packId2 == RESOURCE_REF_CORE_ID) {
        return CheckDependencyVersion(dataPackManifest1->packDependencies, packId2, CORE_DATA_PACK_VERSION_NUMBER);
    }
    const DataPackManifest *dataPackManifest2 = nullptr;
    DataPack *dataPack2 = this->Find(packId2);
    if (dataPack2 == nullptr) {
        return false;
    }
    dataPackManifest2 = dataPack2->GetManifest();
    if (dataPackManifest2 == nullptr) {
        return false;
    }
    bool match = false;
    for (auto &packDependency: dataPackManifest1->packDependencies) {
        if (packDependency.packIdUint == packId2 && dataPackManifest2->versionNumber >= packDependency.minVersion) {
            //If the dependencies within data packet 1 include the ID of data packet 2.
            //如果数据包1内的依赖项包含数据包2的id。
            //If the dependency version in package 1 is less than or equal to the version of package 2.
            //如果包1内的依赖版本小于或等于数据包2的版本。
            match = true;
            break;
        }
    }
    return match;
}
