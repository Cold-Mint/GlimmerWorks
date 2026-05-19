//
// Created by coldmint on 2026/4/16.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"


namespace glimmer {
    class LightSourceManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<
            LightSourceResource> > >
        lightSourceMap_
                {};

        void RegisterCoreLightSourceResource(const std::string &resourceId, const std::string &colorResKey,
                                             uint8_t lightRadius);

    public:
        LightSourceManager();

        LightSourceResource *Register(std::unique_ptr<LightSourceResource> lightSourceResource);

        [[nodiscard]] LightSourceResource *FindLightSourceResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetLightSourceResourceList() const;

        std::string ListLightSourceResource() const;
    };
}
