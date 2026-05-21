//
// Created by Cold-Mint on 2026/2/7.
//

#pragma once

#include "IStructureGenerator.h"

namespace glimmer {
    class TreeStructureGenerator : public IStructureGenerator {
    public:
        void SetWorldSeed(int worldSeed) override;

        std::optional<StructureInfo>
        Generate(TileVector2D startPosition, IStructureResource *structureResource) override;

        [[nodiscard]] StructureGeneratorType GetStructureGeneratorType() const override;
    };
}
