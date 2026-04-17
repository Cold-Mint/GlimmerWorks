//
// Created by coldmint on 2026/4/16.
//

#ifndef GLIMMERWORKS_FIXEDCOLORMANAGER_H
#define GLIMMERWORKS_FIXEDCOLORMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "core/mod/Resource.h"


namespace glimmer {
    class FixedColorManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<FixedColorResource> > >
        fixedColorMap_
                {};

        std::unique_ptr<FixedColorResource> lightMaskFillColorResource_;
        std::unique_ptr<FixedColorResource> lightMaskNoneColorResource_;
        std::unique_ptr<FixedColorResource> lightNoneColorResource_;

    public:
        FixedColorManager();

        FixedColorResource *Register(std::unique_ptr<FixedColorResource> fixedColorResource);

        [[nodiscard]] FixedColorResource *FindFixedColorResource(const std::string &packId, const std::string &key);

        std::vector<std::string> GetFixedColorResourceList() const;

        std::string ListFixedColorResources() const;
    };
}


#endif //GLIMMERWORKS_FIXEDCOLORMANAGER_H
