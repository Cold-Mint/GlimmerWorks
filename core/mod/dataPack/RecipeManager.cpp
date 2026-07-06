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
#include "RecipeManager.h"

#include "google/protobuf/compiler/csharp/csharp_field_base.h"

glimmer::RecipeResource* glimmer::RecipeManager::RegisterRecipe(std::unique_ptr<RecipeResource> recipeResource)
{
    auto& slot =
        recipeMap_[recipeResource->packId][recipeResource->resourceId];
    slot = std::move(recipeResource);
    RecipeResource* recipe = slot.get();
    auto recipeGroup = static_cast<RecipeGroup>(recipe->recipeGroup);
    auto& groupVec = recipeGroupMap_[recipeGroup]; // 不存在则自动新建vector
    groupVec.emplace_back(recipe);

    return recipe;
}

void glimmer::RecipeManager::PreSortRecipes()
{
    for (auto& [group, recipeList] : recipeGroupMap_)
    {
        // Sorting rules:
        // 1. First, sort by the smallest technological level from smallest to largest.
        // 2. If the technological levels are the same, then sort by the quantity of input materials from smallest to largest.
        // 排序规则：
        // 1. 先按最小科技等级从小到大
        // 2. 科技等级相同时，按输入材料数量从小到大
        std::sort(recipeList.begin(), recipeList.end(),
                  [](const RecipeResource* a, const RecipeResource* b)
                  {
                      if (a->minTechnologyLevel != b->minTechnologyLevel)
                      {
                          return a->minTechnologyLevel < b->minTechnologyLevel;
                      }
                      return a->input.size() < b->input.size();
                  });
    }
}

bool glimmer::RecipeManager::IsRecipeSatisfied(const RecipeResource* recipe,
                                               const std::unordered_map<uint64_t, uint8_t>& tagValueMap)
{
    for (const auto& required : recipe->input)
    {
        const auto valueIt = tagValueMap.find(required.GetCachedTagId());
        if (valueIt == tagValueMap.end())
        {
            return false;
        }
        if (valueIt->second < required.requiredWeight)
        {
            return false;
        }
    }
    return true;
}

std::vector<glimmer::RecipeResource*> glimmer::RecipeManager::FindUnlockedRecipes(
    std::unordered_map<RecipeGroup, uint8_t> technologyMap, const std::vector<ItemTagResource*>& totalTagVector) const
{
    std::unordered_map<uint64_t, uint8_t> tagValueMap;
    tagValueMap.reserve(totalTagVector.size());
    for (const auto& tag : totalTagVector)
    {
        if (tag == nullptr)
        {
            continue;
        }
        tagValueMap[tag->GetCachedTagId()] = tag->value;
    }

    std::vector<RecipeResource*> unlockedRecipes;
    for (const auto& [group, recipeList] : recipeGroupMap_)
    {
        const auto techIt = technologyMap.find(group);
        const uint8_t techLevel = techIt != technologyMap.end() ? techIt->second : 0;

        for (const auto& recipe : recipeList)
        {
            if (recipe == nullptr)
            {
                continue;
            }
            if (techLevel < recipe->minTechnologyLevel)
            {
                continue;
            }
            if (!IsRecipeSatisfied(recipe, tagValueMap))
            {
                continue;
            }
            unlockedRecipes.push_back(recipe);
        }
    }

    return unlockedRecipes;
}


glimmer::RecipeResource* glimmer::RecipeManager::FindRecipeResource(const std::string& packId, const std::string& key)
{
    const auto packIt = recipeMap_.find(packId);
    if (packIt == recipeMap_.end())
    {
        return nullptr;
    }

    auto& keyMap = packIt->second;
    const auto keyIt = keyMap.find(key);
    if (keyIt == keyMap.end())
    {
        return nullptr;
    }
    return keyIt->second.get();
}


std::vector<std::string> glimmer::RecipeManager::GetRecipeResourceList() const
{
    std::vector<std::string> result;
    for (const auto& [packId, keyMap] : recipeMap_)
    {
        for (const auto& [key, recipe] : keyMap)
        {
            result.emplace_back(Resource::GenerateId(packId, key));
        }
    }
    return result;
}


std::string glimmer::RecipeManager::ListRecipeResources() const
{
    std::stringstream oss;
    for (const auto& [packId, keyMap] : recipeMap_)
    {
        for (const auto& [key, recipe] : keyMap)
        {
            oss << Resource::GenerateId(packId, key) << "\n";
        }
    }
    return oss.str();
}
