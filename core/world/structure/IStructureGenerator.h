//
// Created by Cold-Mint on 2026/2/7.
//

#pragma once
#include "StructureGeneratorType.h"
#include "StructureInfo.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class IStructureGenerator {
    protected:
        int worldSeed_ = 0;

    public:
        virtual ~IStructureGenerator() = default;

        virtual void SetWorldSeed(int worldSeed);

        /**
         *
         * 生成结构
         * @param startPosition
         * @param structureResource
         * @return
         */
        virtual std::optional<StructureInfo> Generate(TileVector2D startPosition,
                                                      IStructureResource *structureResource) = 0;

        /**
         * Get the structure generator ID
         * 获取结构生成器Id
         * @return
         */
        [[nodiscard]] virtual StructureGeneratorType GetStructureGeneratorType() const = 0;
    };
}
