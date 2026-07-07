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
#include "ItemManager.h"


glimmer::ComposableItemResource* glimmer::ItemManager::AddComposableResource(
    std::unique_ptr<ComposableItemResource> itemResource)
{
    auto& slot =
        composableItemMap_[itemResource->packId][itemResource->resourceId];
    slot = std::move(itemResource);
    return slot.get();
}

glimmer::MaterialItemResource* glimmer::ItemManager::AddMaterialItemResource(
    std::unique_ptr<MaterialItemResource> itemResource)
{
    auto& slot =
        materialItemMap_[itemResource->packId][itemResource->resourceId];
    slot = std::move(itemResource);
    return slot.get();
}


std::unique_ptr<glimmer::ComposableItemResource> glimmer::ItemManager::CreatePlaceholderComposableItemResource(
    const std::string& packId, const std::string& resourceId)
{
    auto composableItemResource = std::make_unique<ComposableItemResource>();
    composableItemResource->packId = packId;
    composableItemResource->resourceId = resourceId;
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TEXTURE);
    resourceRef.SetResourceKey(ERROR_TEXTURE_KEY);
    composableItemResource->texture = resourceRef;
    composableItemResource->defaultAbilityList = {};
    composableItemResource->slotSize = 0;
    composableItemResource->missing = true;
    return composableItemResource;
}

std::unique_ptr<glimmer::AbilityItemResource> glimmer::ItemManager::CreateAbilityItemResource(const std::string& packId,
    const std::string& resourceId)
{
    auto abilityItemResource = std::make_unique<AbilityItemResource>();
    abilityItemResource->packId = packId;
    abilityItemResource->resourceId = resourceId;
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TEXTURE);
    resourceRef.SetResourceKey(ERROR_TEXTURE_KEY);
    abilityItemResource->texture = resourceRef;
    abilityItemResource->ability = ABILITY_ID_NONE;
    abilityItemResource->missing = true;
    return abilityItemResource;
}

std::unique_ptr<glimmer::MaterialItemResource> glimmer::ItemManager::CreateMaterialItemResource(
    const std::string& packId, const std::string& resourceId)
{
    auto materialItemResource = std::make_unique<MaterialItemResource>();
    materialItemResource->packId = packId;
    materialItemResource->resourceId = resourceId;
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_TEXTURE);
    resourceRef.SetResourceKey(ERROR_TEXTURE_KEY);
    materialItemResource->texture = resourceRef;
    materialItemResource->missing = true;
    return materialItemResource;
}


glimmer::AbilityItemResource* glimmer::ItemManager::AddAbilityItemResource(
    std::unique_ptr<AbilityItemResource> itemResource)
{
    auto& slot =
        abilityItemMap_[itemResource->packId][itemResource->resourceId];
    slot = std::move(itemResource);
    return slot.get();
}


glimmer::ComposableItemResource* glimmer::ItemManager::FindComposableItemResource(
    const std::string& packId, const std::string& key)
{
    const auto packIt = composableItemMap_.find(packId);
    if (packIt == composableItemMap_.end())
    {
        return AddComposableResource(CreatePlaceholderComposableItemResource(packId, key));
    }

    auto& keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end())
    {
        return AddComposableResource(CreatePlaceholderComposableItemResource(packId, key));
    }

    return keyIt->second.get();
}

glimmer::MaterialItemResource* glimmer::ItemManager::FindMaterialItemResource(const std::string& packId,
                                                                              const std::string& key)
{
    const auto packIt = materialItemMap_.find(packId);
    if (packIt == materialItemMap_.end())
    {
        return AddMaterialItemResource(CreateMaterialItemResource(packId, key));
    }

    auto& keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end())
    {
        return AddMaterialItemResource(CreateMaterialItemResource(packId, key));
    }

    return keyIt->second.get();
}

glimmer::AbilityItemResource* glimmer::ItemManager::FindAbilityItemResource(const std::string& packId,
                                                                            const std::string& key)
{
    const auto packIt = abilityItemMap_.find(packId);
    if (packIt == abilityItemMap_.end())
    {
        return AddAbilityItemResource(CreateAbilityItemResource(packId, key));
    }

    auto& keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end())
    {
        return AddAbilityItemResource(CreateAbilityItemResource(packId, key));
    }
    return keyIt->second.get();
}

std::vector<std::string> glimmer::ItemManager::GetComposableItemIDList()
{
    std::vector<std::string> result;
    for (const auto& [packId, keyMap] : composableItemMap_)
    {
        for (const auto& [key, resource] : keyMap)
        {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::vector<std::string> glimmer::ItemManager::GetMaterialItemIDList()
{
    std::vector<std::string> result;
    for (const auto& [packId, keyMap] : materialItemMap_)
    {
        for (const auto& [key, resource] : keyMap)
        {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::vector<std::string> glimmer::ItemManager::GetAbilityItemIDList()
{
    std::vector<std::string> result;
    for (const auto& [packId, keyMap] : abilityItemMap_)
    {
        for (const auto& [key, resource] : keyMap)
        {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}

std::string glimmer::ItemManager::ListComposableItems() const
{
    std::ostringstream oss;
    for (const auto& [packId, keyMap] : composableItemMap_)
    {
        for (const auto& [key, _] : keyMap)
        {
            oss << Resource::GenerateId(packId, key) << '\n';
        }
    }
    return oss.str();
}

std::string glimmer::ItemManager::ListMaterialItems() const
{
    std::ostringstream oss;
    for (const auto& [packId, keyMap] : materialItemMap_)
    {
        for (const auto& [key, _] : keyMap)
        {
            oss << Resource::GenerateId(packId, key) << '\n';
        }
    }
    return oss.str();
}

std::string glimmer::ItemManager::ListAbilityItems() const
{
    std::ostringstream oss;
    for (const auto& [packId, keyMap] : abilityItemMap_)
    {
        for (const auto& [key, _] : keyMap)
        {
            oss << Resource::GenerateId(packId, key) << '\n';
        }
    }
    return oss.str();
}
