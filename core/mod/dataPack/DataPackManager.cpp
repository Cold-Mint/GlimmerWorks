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

#include <regex>
#include <algorithm>
#include "DataPack.h"
#include "core/context/AppContext.h"
#include "toml11/spec.hpp"

namespace glimmer
{
    class Config;
}

namespace fs = std::filesystem;

bool glimmer::DataPackManager::IsDataPackAvailable(const DataPack& pack) const
{
    const PackManifest& manifest = pack.GetManifest();
    if (manifest.id == RESOURCE_REF_CORE || manifest.id == RESOURCE_REF_SELF)
    {
        return false;
    }
    for (const auto& packId : packManifestVector_)
    {
        if (packId.id == manifest.id)
        {
            return false;
        }
    }
    if (manifest.minGameVersion > GAME_VERSION_NUMBER)
    {
        return false;
    }
    return !manifest.resPack;
}

bool glimmer::DataPackManager::IsDataPackEnabled(const DataPack& pack,
                                                 const std::vector<std::string>& enabledDataPack)
{
    return std::ranges::find(enabledDataPack, pack.GetManifest().id) != enabledDataPack.end();
}

bool glimmer::DataPackManager::CheckDependencyVersion(const std::vector<PackDependence>& dependencies,
                                                      const std::string_view packId, const uint32_t version)
{
    return std::ranges::any_of(dependencies, [&](const auto& dep)
    {
        return dep.packId == packId && version >= dep.minVersion;
    });
}

glimmer::DataPackManager::DataPackManager(VirtualFileSystem* virtualFilesystem,
                                          TomlTemplateExpander* tomlTemplateExpander) : virtualFileSystem_(
        virtualFilesystem), tomlTemplateExpander_(tomlTemplateExpander)
{
}

bool glimmer::DataPackManager::IsDependencySatisfied(const std::string& pack1Id, const std::string& pack2Id)
{
    if (pack1Id == pack2Id)
    {
        return true;
    }
    const DataPackManifest* dataPackManifest1 = nullptr;
    for (auto& packId : packManifestVector_)
    {
        if (packId.id == pack1Id)
        {
            dataPackManifest1 = &packId;
            break;
        }
    }
    if (dataPackManifest1 == nullptr)
    {
        return false;
    }
    if (pack2Id == RESOURCE_REF_CORE)
    {
        return CheckDependencyVersion(dataPackManifest1->packDependencies, pack2Id, CORE_DATA_PACK_VERSION_NUMBER);
    }
    DataPackManifest* dataPackManifest2 = nullptr;
    for (auto& packId : packManifestVector_)
    {
        if (packId.id == pack2Id)
        {
            dataPackManifest2 = &packId;
            break;
        }
    }
    if (dataPackManifest2 == nullptr)
    {
        return false;
    }
    bool match = false;
    for (auto& packDependency : dataPackManifest1->packDependencies)
    {
        if (packDependency.packId == pack2Id && dataPackManifest2->versionNumber >= packDependency.minVersion)
        {
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

bool glimmer::DataPackManager::Contains(const std::string& packId) const
{
    return packVerifyStateMap_.contains(packId);
}

glimmer::PackVerifyState glimmer::DataPackManager::GetPackVerifyState(const std::string& packId)
{
    if (packVerifyStateMap_.contains(packId))
    {
        return packVerifyStateMap_[packId];
    }
    return PackVerifyState::Unsigned;
}

std::vector<std::string> glimmer::DataPackManager::GetPackIdList() const
{
    std::vector<std::string> packIdList;
    for (auto& packManifest : packManifestVector_)
    {
        packIdList.push_back(packManifest.id);
    }
    return packIdList;
}

int glimmer::DataPackManager::Scan(AppContext* appContext, const toml::spec& tomlVersion)
{
    if (virtualFileSystem_ == nullptr)
    {
        return 0;
    }
    Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return 0;
    }
    const std::filesystem::path& dataPackPath = config->mods.dataPackPath;
    if (!virtualFileSystem_->Exists(dataPackPath))
    {
        return 0;
    }
    packManifestVector_.clear();
    packVerifyStateMap_.clear();

    int success = 0;
    for (const std::vector<std::filesystem::path> files = virtualFileSystem_->ListFile(dataPackPath, false); const auto&
         entry : files)
    {
        if (!virtualFileSystem_->IsFile(entry))
        {
            DataPack pack(entry, virtualFileSystem_, tomlTemplateExpander_, tomlVersion);
            if (!pack.LoadManifest())
            {
                continue;
            }
            // Determine whether the data packet is enabled
            // 判断数据包是否启用
            if (!IsDataPackEnabled(pack, appContext->GetConfig()->mods.enabledDataPack))
            {
                continue;
            }
            if (!IsDataPackAvailable(pack))
            {
                continue;
            }
            if (pack.LoadPack(appContext))
            {
                success++;
                packVerifyStateMap_[pack.GetManifest().id] = pack.GetPackVerifyState();
                packManifestVector_.push_back(pack.GetManifest());
            }
        }
    }
    return success;
}
