//
// Created by Cold-Mint on 2026/2/13.
//

#pragma once
#include "core/mod/Resource.h"

namespace glimmer {
    class StructureManager {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<IStructureResource> > >
        structureMap_
                {};
        std::vector<IStructureResource *> structureVector_ = {};

    public:
        IStructureResource *AddResource(std::unique_ptr<IStructureResource> structureResource);

        [[nodiscard]] IStructureResource *Find(const std::string &packId, const std::string &key);

        [[nodiscard]] const std::vector<IStructureResource *> &GetAll() const;

        [[nodiscard]] std::vector<std::string> GetStructureIDList() const;

        std::string ListStructures() const;
    };
}
