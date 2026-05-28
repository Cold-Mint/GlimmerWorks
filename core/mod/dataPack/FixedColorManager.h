//
// Created by Cold-Mint on 2026/4/16.
//

#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"


namespace glimmer {
    class FixedColorManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<FixedColorResource> > >
        fixedColorMap_
                {};

        void RegisterCoreRef(const std::string &resourceId, uint8_t r, uint8_t b, uint8_t g, uint8_t a);

    public:
        FixedColorManager();

        FixedColorResource *Register(std::unique_ptr<FixedColorResource> fixedColorResource);

        [[nodiscard]] FixedColorResource *FindFixedColorResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetFixedColorResourceList() const;

        std::string ListFixedColorResources() const;
    };
}
