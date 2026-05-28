//
// Created by Cold-Mint on 2026/4/16.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"

namespace glimmer {
    class LightMaskManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<LightMaskResource> > >
        lightMaskMap_
                {};

        LightMaskResource *RegisterCoreLightMaskResource(const std::string &resourceId,
                                                         const std::string &colorKey);

    public:
        LightMaskManager();

        LightMaskResource *Register(std::unique_ptr<LightMaskResource> lightMaskResource);

        [[nodiscard]] LightMaskResource *FindLightMaskResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetLightMaskResourceList() const;

        std::string ListLightMaskResource() const;
    };
}
