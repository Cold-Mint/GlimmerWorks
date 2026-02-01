//
// Created by Cold-Mint on 2025/10/9.
//
#include "Resource.h"

#include <random>

std::vector<glimmer::ResourceRef> glimmer::LootResource::GetLootItems(const LootResource *lootResource) {
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t totalWeight = lootResource->empty_weight;
    uint32_t totalPoolWeight = 0;
    for (auto &pool: lootResource->pool) {
        totalWeight += pool.weight;
        totalPoolWeight += pool.weight;
    }
    std::vector<ResourceRef> resourceRefs = {};
    for (auto &mandatory: lootResource->mandatory) {
        ResourceRef ref = mandatory.item;
        ResourceRefArg refArg;
        refArg.SetName("amount");
        std::uniform_int_distribution dist(mandatory.min, mandatory.max);
        uint32_t randomValue = dist(gen);
        refArg.SetDataFromInt(static_cast<int>(randomValue));
        ref.AddArg(refArg);
        resourceRefs.push_back(ref);
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
                    ResourceRef ref = pool.item;
                    ResourceRefArg refArg;
                    refArg.SetName("amount");
                    std::uniform_int_distribution poolDist(pool.min, pool.max);
                    uint32_t poolRandomValue = poolDist(gen);
                    refArg.SetDataFromInt(static_cast<int>(poolRandomValue));
                    ref.AddArg(refArg);
                    resourceRefs.push_back(ref);
                    break;
                }
            }
        }
    }
    return resourceRefs;
}
