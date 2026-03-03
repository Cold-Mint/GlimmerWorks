//
// Created by coldmint on 2026/3/3.
//

#ifndef GLIMMERWORKS_MOBMANAGER_H
#define GLIMMERWORKS_MOBMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "core/ecs/GameEntity.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class MobManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<MobResource> > > mobMap_
                {};


    public:
        MobResource *AddResource(std::unique_ptr<MobResource> mobResource);

        [[nodiscard]] MobResource *FindMobResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetMobList() const;

        std::string ListMobs() const;
    };
}

#endif //GLIMMERWORKS_MOBMANAGER_H
