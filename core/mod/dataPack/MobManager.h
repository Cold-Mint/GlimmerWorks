//
// Created by Cold-Mint on 2026/3/3.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"

namespace glimmer {
    class MobManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<MobResource> > > mobMap_
                {};

        std::vector<MobResource *> playerMobsResource_;

    public:
        MobResource *Register(std::unique_ptr<MobResource> mobResource);

        [[nodiscard]] MobResource *FindMobResource(const std::string &packId, const std::string &key);

        /**
         * FindPlayerResource
         * 获取玩家资源列表
         * @return
         */
        [[nodiscard]] const std::vector<MobResource *> &GetPlayerResourceList() const;

        std::vector<std::string> GetMobList() const;

        std::string ListMobs() const;
    };
}
