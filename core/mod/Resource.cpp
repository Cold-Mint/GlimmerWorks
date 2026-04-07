//
// Created by Cold-Mint on 2025/10/9.
//
#include "Resource.h"

#include <random>

std::string glimmer::Resource::GenerateId(const std::string &packId, const std::string &key) {
    return packId + ":" + key;
}

std::string glimmer::Resource::GenerateId(const Resource &resource) {
    return GenerateId(resource.packId, resource.resourceId);
}

glimmer::VariableDefinitionType glimmer::VariableDefinition::ResolveVariableType(const std::string &typeName) {
    const auto it = variableDefinitionTypeMap_.find(typeName);
    return it == variableDefinitionTypeMap_.end() ? INT : it->second;
}

void glimmer::VariableDefinition::AsResourceRef(ResourceRef &resourceRef) const {
    if (type != REF) {
        return;
    }
    ResourceRefMessage refMessage;
    if (refMessage.ParseFromString(value)) {
        resourceRef.ReadResourceRefMessage(refMessage);
    }
}

int glimmer::VariableDefinition::AsInt() const {
    if (type != INT) {
        return 0;
    }
    return std::stoi(value);
}

float glimmer::VariableDefinition::AsFloat() const {
    if (type != FLOAT) {
        return 0.0F;
    }
    return std::stof(value);
}

bool glimmer::VariableDefinition::AsBool() const {
    if (type != BOOL) {
        return false;
    }
    if (value == "1" || value == "true" || value == "yes" || value == "y") {
        return true;
    }
    if (value == "0" || value == "false" || value == "no" || value == "n") {
        return false;
    }
    return false;
}

std::string glimmer::VariableDefinition::AsString() const {
    if (type != STRING) {
        return "";
    }
    return value;
}

const glimmer::VariableDefinition *glimmer::VariableConfig::FindVariable(const std::string &name) const {
    for (auto &data: definition) {
        if (data.key == name) {
            return &data;
        }
    }
    return nullptr;
}

glimmer::VariableDefinition *glimmer::VariableConfig::FindVariableModifiable(const std::string &name) {
    for (auto &data: definition) {
        if (data.key == name) {
            return &data;
        }
    }
    return nullptr;
}

void glimmer::VariableConfig::UpdateArgs(const std::string &selfPackId) {
    for (auto &data: definition) {
        if (data.type == REF) {
            ResourceRef resourceRef;
            data.AsResourceRef(resourceRef);
            resourceRef.SetSelfPackageId(selfPackId);
            ResourceRefMessage refMessage;
            resourceRef.WriteResourceRefMessage(refMessage);
            data.value = refMessage.SerializeAsString();
        }
    }
}

SDL_Color glimmer::ColorResource::ToSDLColor() const {
    return SDL_Color{r, g, b, a};
}

FastNoiseLite *glimmer::MineralBiomeDecoratorResource::GetFastNoiseLite(const int seed) {
    if (fastNoiseLite_ == nullptr) {
        fastNoiseLite_ = std::make_unique<FastNoiseLite>();
    }
    fastNoiseLite_->SetSeed(seed + seedOffset);
    fastNoiseLite_->SetNoiseType(static_cast<FastNoiseLite::NoiseType>(noiseType));
    fastNoiseLite_->SetFrequency(frequency);
    return fastNoiseLite_.get();
}

std::vector<ItemMessage> glimmer::LootResource::GetLootItems(const LootResource *lootResource) {
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t totalWeight = lootResource->empty_weight;
    uint32_t totalPoolWeight = 0;
    for (auto &pool: lootResource->pool) {
        totalWeight += pool.weight;
        totalPoolWeight += pool.weight;
    }
    std::vector<ItemMessage> itemMessageList = {};
    for (auto &mandatory: lootResource->mandatory) {
        ItemMessage itemMessage;
        std::uniform_int_distribution dist(mandatory.min, mandatory.max);
        const uint32_t randomValue = dist(gen);
        itemMessage.set_amount(randomValue);
        ResourceRefMessage &resourceRefMessage = *itemMessage.mutable_itemresourceref();
        mandatory.item.WriteResourceRefMessage(resourceRefMessage);
        itemMessageList.push_back(itemMessage);
    }

    if (totalPoolWeight > 0) {
        for (int r = 0; r < lootResource->rolls; r++) {
            std::uniform_int_distribution<uint32_t> rollDist(0, totalWeight - 1);
            uint32_t rollsRandomValue = rollDist(gen);
            if (rollsRandomValue <= lootResource->empty_weight) {
                //This roll doesn't lose anything.
                //本次 roll 什么都不掉
                continue;
            }
            uint32_t currentWeight = 0;
            for (auto &pool: lootResource->pool) {
                currentWeight += pool.weight;
                if (rollsRandomValue <= currentWeight) {
                    ItemMessage itemMessage;
                    std::uniform_int_distribution dist(pool.min, pool.max);
                    const uint32_t randomValue = dist(gen);
                    itemMessage.set_amount(randomValue);
                    ResourceRefMessage &resourceRefMessage = *itemMessage.mutable_itemresourceref();
                    pool.item.WriteResourceRefMessage(resourceRefMessage);
                    itemMessageList.push_back(itemMessage);
                    break;
                }
            }
        }
    }
    return itemMessageList;
}
