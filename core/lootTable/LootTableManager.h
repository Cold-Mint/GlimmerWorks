//
// Created by Cold-Mint on 2026/1/30.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"

namespace glimmer {
    class LootTableManager {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<LootResource> > > lootMap_{};

    public:
        LootResource *AddResource(std::unique_ptr<LootResource> lootResource);

        [[nodiscard]] LootResource *Find(const std::string &packId, const std::string &key);

        std::vector<std::string> GetLootTableList();

        std::string ListLootTables() const;
    };
}
