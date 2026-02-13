//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
#define GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
#include "IStructureGenerator.h"

namespace glimmer {
    class StaticStructureGenerator : public IStructureGenerator {
        uint32_t width_ = 0;
        std::vector<ResourceRef> tileData_;

    public:
        /**
         * Set the structure width
         * 设置结构宽度
         * @param width
         */
        void SetWidth(uint32_t width);

        /**
         * SetTileData
         * 设置瓦片数据
         * @param tileData
         */
        void SetTileData(const std::vector<ResourceRef> &tileData);


        StructureInfo Generate(TileVector2D startPosition) override;

        std::string GetStructureGeneratorId() override;
    };
}

#endif //GLIMMERWORKS_STATICSTRUCTUREGENERATOR_H
