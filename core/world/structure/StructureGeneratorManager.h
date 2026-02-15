//
// Created by coldmint on 2026/2/13.
//

#ifndef GLIMMERWORKS_STRUCTUREGENERATORMANAGER_H
#define GLIMMERWORKS_STRUCTUREGENERATORMANAGER_H
#include "IStructureGenerator.h"
#include "StructureInfo.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class StructureGeneratorManager {
        std::unordered_map<std::string, std::unique_ptr<IStructureGenerator> > structureGeneratorMap_{};

    public:
        void SetWorldSeed(const int worldSeed);

        void RegisterStructureGenerator(std::unique_ptr<IStructureGenerator> structureGenerator);

        std::optional<StructureInfo> Generate(TileVector2D startPosition, StructureResource *structureResource);
    };
}

#endif //GLIMMERWORKS_STRUCTUREGENERATORMANAGER_H
