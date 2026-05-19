//
// Created by Cold-Mint on 2026/2/13.
//

#pragma once
#include "IStructureGenerator.h"
#include "StructureInfo.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class StructureGeneratorManager {
        std::unordered_map<std::string, std::unique_ptr<IStructureGenerator> > structureGeneratorMap_{};

    public:
        void SetWorldSeed(const int worldSeed);

        void RegisterStructureGenerator(std::unique_ptr<IStructureGenerator> structureGenerator);

        std::optional<StructureInfo> Generate(TileVector2D structuralOrigin, StructureResource *structureResource);
    };
}
