//
// Created by Cold-Mint on 2026/2/7.
//

#pragma once
#include "IStructureGenerator.h"

namespace glimmer {
    class StaticStructureGenerator : public IStructureGenerator {
    public:
        StructureInfo Generate(TileVector2D startPosition, StructureResource *structureResource) override;

        std::string GetStructureGeneratorId() override;
    };
}
