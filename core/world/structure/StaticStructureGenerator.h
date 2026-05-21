//
// Created by Cold-Mint on 2026/2/7.
//

#pragma once
#include "IStructureGenerator.h"

namespace glimmer {
    class StaticStructureGenerator : public IStructureGenerator {
    public:
        std::optional<StructureInfo>
        Generate(TileVector2D startPosition, IStructureResource *structureResource) override;

        [[nodiscard]] StructureGeneratorType GetStructureGeneratorType() const override;
    };
}
