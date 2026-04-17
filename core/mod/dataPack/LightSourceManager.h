//
// Created by coldmint on 2026/4/16.
//

#ifndef GLIMMERWORKS_LIGHTSOURCEMANAGER_H
#define GLIMMERWORKS_LIGHTSOURCEMANAGER_H
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

        std::unique_ptr<LightSourceResource> lightSourceNoneResource_;

    public:
        LightSourceManager();

        LightSourceResource *Register(std::unique_ptr<LightSourceResource> lightSourceResource);

        [[nodiscard]] LightSourceResource *FindLightSourceResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetLightSourceResourceList() const;

        std::string ListLightSourceResource() const;
    };
}


#endif //GLIMMERWORKS_LIGHTSOURCEMANAGER_H
