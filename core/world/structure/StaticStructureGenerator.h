//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
#define GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
#include "IStructureGenerator.h"

namespace glimmer {
    class StaticStructureGenerator : public IStructureGenerator {
    public:
        StructureInfo Generate(TileVector2D startPosition, StructureResource *structureResource) override;

        std::string GetStructureGeneratorId() override;
    };
}

#endif //GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
