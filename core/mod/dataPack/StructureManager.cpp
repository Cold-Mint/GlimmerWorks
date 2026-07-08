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
#include "StructureManager.h"

#include "core/log/LogCat.h"

glimmer::IStructureResource* glimmer::StructureManager::AddResource(
    std::unique_ptr<IStructureResource> structureResource)
{
    LogCat::i("Registering tile resource: packId = ", structureResource->packId,
              ", resourceId = ", structureResource->resourceId);
    auto& slot = structureMap_[structureResource->packId][structureResource->resourceId];
    slot = std::move(structureResource);
    structureVector_.push_back(slot.get());
    return slot.get();
}

glimmer::IStructureResource* glimmer::StructureManager::Find(const std::string& packId, const std::string& key)
{
    LogCat::d("Searching for structure resource: packId = ", packId, ", key = ", key);
    const auto packIt = structureMap_.find(packId);
    if (packIt == structureMap_.end())
    {
        LogCat::w("Pack not found: ", packId);
        return nullptr;
    }

    auto& keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end())
    {
        LogCat::w("Key not found in pack ", packId, ": ", key);
        return nullptr;
    }

    LogCat::i("Found structure resource: packId = ", packId, ", key = ", key);
    return keyIt->second.get();
}

std::vector<glimmer::IStructureResource*> glimmer::StructureManager::GetAll()
{
    return structureVector_;
}


std::vector<std::string> glimmer::StructureManager::GetStructureIDList() const
{
    std::vector<std::string> result;
    for (const auto structureVector : structureVector_)
    {
        result.emplace_back(Resource::GenerateId(structureVector->packId, structureVector->resourceId));
    }
    return result;
}

std::string glimmer::StructureManager::ListStructures() const
{
    std::ostringstream oss;
    for (const auto& structurePtr : structureVector_)
    {
        if (!structurePtr)
        {
            continue;
        }
        oss << Resource::GenerateId(structurePtr->packId, structurePtr->resourceId) << "\n";
    }
    return oss.str();
}
