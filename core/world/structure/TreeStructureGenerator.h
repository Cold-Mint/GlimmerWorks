//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_TREESTRUCTUREGENERATOR_H
#define GLIMMERWORKS_TREESTRUCTUREGENERATOR_H

#include "IStructureGenerator.h"

namespace glimmer {
    class TreeStructureGenerator : public IStructureGenerator {
    public:
        void SetWorldSeed(int worldSeed) override;

        StructureInfo Generate(TileVector2D startPosition, StructureResource *structureResource) override;


        std::string GetStructureGeneratorId() override;
    };
}


#endif //GLIMMERWORKS_TREESTRUCTUREGENERATOR_H
