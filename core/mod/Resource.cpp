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

glimmer::VariableDefinitionType glimmer::VariableDefinition::ResolveVariableType(const std::string& typeName)
{
    const auto it = variableDefinitionTypeMap_.find(typeName);
    return it == variableDefinitionTypeMap_.end() ? INT : it->second;
}

void glimmer::VariableDefinition::AsResourceRef(ResourceRef& resourceRef) const
{
    if (type != REF)
    {
        return;
    }
    ResourceRefMessage refMessage;
    if (refMessage.ParseFromString(value))
    {
        resourceRef.ReadResourceRefMessage(refMessage);
    }
}

int glimmer::VariableDefinition::AsInt() const
{
    if (type != INT)
    {
        return 0;
    }
    return std::stoi(value);
}

float glimmer::VariableDefinition::AsFloat() const
{
    if (type != FLOAT)
    {
        return 0.0F;
    }
    return std::stof(value);
}

bool glimmer::VariableDefinition::AsBool() const
{
    if (type != BOOL)
    {
        return false;
    }
    if (value == "1" || value == "true" || value == "yes" || value == "y")
    {
        return true;
    }
    if (value == "0" || value == "false" || value == "no" || value == "n")
    {
        return false;
    }
    return false;
}

std::string glimmer::VariableDefinition::AsString() const
{
    if (type != STRING)
    {
        return "";
    }
    return value;
}

const glimmer::VariableDefinition* glimmer::VariableConfig::FindVariable(const std::string& name) const
{
    for (auto& data : definition)
    {
        if (data.key == name)
        {
            return &data;
        }
    }
    return nullptr;
}

glimmer::VariableDefinition* glimmer::VariableConfig::FindVariableModifiable(const std::string& name)
{
    for (auto& data : definition)
    {
        if (data.key == name)
        {
            return &data;
        }
    }
    return nullptr;
}

void glimmer::VariableConfig::UpdateArgs(const std::string& selfPackId)
{
    for (auto& data : definition)
    {
        if (data.type == REF)
        {
            ResourceRef resourceRef;
            data.AsResourceRef(resourceRef);
            resourceRef.SetSelfPackageId(selfPackId);
            ResourceRefMessage refMessage;
            resourceRef.WriteResourceRefMessage(refMessage);
            data.value = refMessage.SerializeAsString();
        }
    }
}

glimmer::Color glimmer::ColorResource::ToColor() const
{
    return Color{r, g, b, a};
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
        itemMessageList.push_back(itemMessage);
    }

    if (totalPoolWeight > 0)
    {
        for (int r = 0; r < lootResource->rolls; r++)
        {
            auto rollsRandomValue = RandomUtils::Random<uint32_t>(0, totalWeight - 1);
            if (rollsRandomValue <= lootResource->empty_weight)
            {
                //This roll doesn't lose anything.
                //本次 roll 什么都不掉
                continue;
            }
            uint32_t currentWeight = 0;
            for (auto& pool : lootResource->pool)
            {
                currentWeight += pool.weight;
                if (rollsRandomValue <= currentWeight)
                {
                    ItemMessage itemMessage;
                    const auto randomValue = RandomUtils::Random<uint32_t>(pool.min, pool.max);
                    itemMessage.set_amount(randomValue);
                    ResourceRefMessage& resourceRefMessage = *itemMessage.mutable_itemresourceref();
                    pool.item.WriteResourceRefMessage(resourceRefMessage);
                    itemMessageList.push_back(itemMessage);
                    break;
                }
            }
        }
    }
    return itemMessageList;
}

void glimmer::RequiredTag::MakeCachedTag()
{
    if (cachedTagId_ == 0)
    {
        cachedTagId_ = StringUtils::StringToUint64(requiredTag);
    }
}

uint64_t glimmer::RequiredTag::GetCachedTagId() const
{
    return cachedTagId_;
}
