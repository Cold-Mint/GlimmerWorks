//
// Created by coldmint on 2026/2/13.
//

#ifndef GLIMMERWORKS_STRUCTUREMANAGER_H
#define GLIMMERWORKS_STRUCTUREMANAGER_H
#include "core/mod/Resource.h"

namespace glimmer {
    class StructureManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<StructureResource> > >
        structureMap_
                {};

    public:
        StructureResource *AddResource(std::unique_ptr<StructureResource> structureResource);

        [[nodiscard]] StructureResource *Find(const std::string &packId, const std::string &key);

        [[nodiscard]] std::vector<std::string> GetStructureIDList();

        std::string ListStructures();
    };
}

#endif //GLIMMERWORKS_STRUCTUREMANAGER_H
