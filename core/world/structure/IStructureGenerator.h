//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_ISTRUCTUREGENERATOR_H
#define GLIMMERWORKS_ISTRUCTUREGENERATOR_H
#include "StructureInfo.h"
#include "core/mod/Resource.h"

namespace glimmer {
    class IStructureGenerator {
    public:
        virtual ~IStructureGenerator() = default;

        /**
         *
         * 生成结构
         * @param startPosition
         * @param structureResource
         * @return
         */
        virtual StructureInfo Generate(TileVector2D startPosition,
                                       StructureResource *structureResource) = 0;

        /**
         * Get the structure generator ID
         * 获取结构生成器Id
         * @return
         */
        virtual std::string GetStructureGeneratorId() = 0;
    };
}

#endif //GLIMMERWORKS_ISTRUCTUREGENERATOR_H
