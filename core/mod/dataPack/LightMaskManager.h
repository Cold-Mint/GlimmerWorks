//
// Created by coldmint on 2026/4/16.
//

#ifndef GLIMMERWORKS_LIGHTMASKMANAGER_H
#define GLIMMERWORKS_LIGHTMASKMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"

namespace glimmer {
    class LightMaskManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<LightMaskResource> > >
        lightMaskMap_
                {};

        std::unique_ptr<LightMaskResource> lightMaskFillResource_;
        std::unique_ptr<LightMaskResource> lightMaskHighResource_;
        std::unique_ptr<LightMaskResource> lightMaskMediumResource_;
        std::unique_ptr<LightMaskResource> lightMaskLowResource_;
        std::unique_ptr<LightMaskResource> lightMaskNoneResource_;

        static std::unique_ptr<LightMaskResource> CreateLightMaskResource(const std::string &resourceId,
                                                                          const std::string &colorKey);

    public:
        LightMaskManager();

        LightMaskResource *Register(std::unique_ptr<LightMaskResource> lightMaskResource);

        [[nodiscard]] LightMaskResource *FindLightMaskResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetLightMaskResourceList() const;

        std::string ListLightMaskResource() const;
    };
}

#endif //GLIMMERWORKS_LIGHTMASKMANAGER_H
