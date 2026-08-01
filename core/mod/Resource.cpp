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
#include "Resource.h"

#include <random>

#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"

std::string glimmer::Resource::GenerateId(const std::string& packId, const std::string& key)
{
    return packId + ":" + key;
}

std::string glimmer::Resource::GenerateId(const Resource& resource)
{
    return GenerateId(resource.packId, resource.resourceId);
}

glimmer::Color glimmer::FixedColorResource::ToColor() const
{
    return Color{r, g, b, a};
}

const std::unordered_set<std::string>& glimmer::BiomeStructurePlacementConditionsResource::GetCachedBiomeIds() const
{
    return cachedBiomeIds_;
}

void glimmer::BiomeStructurePlacementConditionsResource::RefreshCache()
{
    cachedBiomeIds_.clear();
    for (auto& ref : targetBiomes)
    {
        cachedBiomeIds_.insert(GenerateId(ref.GetPackageId(), ref.GetResourceKey()));
    }
}


glimmer::Color glimmer::ColorResource::ToColor() const
{
    return Color{r, g, b, a};
}

void glimmer::ItemTagResource::MakeCachedTag()
{
    if (cachedTagId == 0)
    {
        cachedTagId = StringUtils::StringToUint64(name);
    }
}

FastNoiseLite* glimmer::MineralBiomeDecoratorResource::GetFastNoiseLite(const int seed)
{
    if (fastNoiseLite_ == nullptr)
    {
        fastNoiseLite_ = std::make_unique<FastNoiseLite>();
    }
    fastNoiseLite_->SetSeed(seed + seedOffset);
    fastNoiseLite_->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(noiseType));
    fastNoiseLite_->SetFrequency(frequency);
    return fastNoiseLite_.get();
}

void glimmer::LootResource::TryRollSingleLoot(uint32_t totalWeight, const LootResource* lootResource,
                                              std::vector<ItemMessage>& itemMessageList)
{
    auto rollsRandomValue = RandomUtils::Random<uint32_t>(0, totalWeight - 1);
    if (rollsRandomValue <= lootResource->empty_weight)
    {
        return;
    }
    uint32_t currentWeight = 0;
    for (auto& pool : lootResource->pool)
    {
        currentWeight += pool.weight;
        if (rollsRandomValue > currentWeight)
        {
            continue;
        }
        ItemMessage itemMessage;
        const auto randomValue = RandomUtils::Random<uint32_t>(pool.min, pool.max);
        itemMessage.set_amount(randomValue);
        ResourceRefMessage& resourceRefMessage = *itemMessage.mutable_itemresourceref();
        pool.item.WriteResourceRefMessage(resourceRefMessage);
        itemMessageList.push_back(itemMessage);
        break;
    }
}

std::vector<ItemMessage> glimmer::LootResource::GetLootItems(const LootResource* lootResource)
{
    uint32_t totalWeight = lootResource->empty_weight;
    uint32_t totalPoolWeight = 0;
    for (auto& pool : lootResource->pool)
    {
        totalWeight += pool.weight;
        totalPoolWeight += pool.weight;
    }
    std::vector<ItemMessage> itemMessageList = {};
    for (auto& mandatory : lootResource->mandatory)
    {
        ItemMessage itemMessage;
        const auto randomValue = RandomUtils::Random<uint32_t>(mandatory.min, mandatory.max);
        itemMessage.set_amount(randomValue);
        ResourceRefMessage& resourceRefMessage = *itemMessage.mutable_itemresourceref();
        mandatory.item.WriteResourceRefMessage(resourceRefMessage);
        itemMessageList.emplace_back(itemMessage);
    }

    if (totalPoolWeight > 0)
    {
        for (int r = 0; r < lootResource->rolls; r++)
        {
            TryRollSingleLoot(totalWeight, lootResource, itemMessageList);
        }
    }
    return itemMessageList;
}

void glimmer::RequiredTag::MakeCachedTag()
{
    if (cachedTagId == 0)
    {
        cachedTagId = StringUtils::StringToUint64(requiredTag);
    }
}
