//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_TREESTRUCTUREGENERATOR_H
#define GLIMMERWORKS_TREESTRUCTUREGENERATOR_H

#include "IStructureGenerator.h"

namespace glimmer {
    class TreeStructureGenerator : public IStructureGenerator {
        /**
         * leafResource
         * 树叶资源
         */
        ResourceRef leafResource;
        /**
         * trunkResource
         * 树干资源
         */
        ResourceRef trunkResource;
        /**
         * Minimum trunk height
         * 最小树干高度
         */
        int trunkMinHeight = 2;
        /**
         * Max trunk height
         * 最大树干高度
         */
        int trunkMaxHeight = 5;

        /**
         * The width of the tree trunk
         * 树干宽度
         */
        int trunkWidth = 1;

        /**
         * Does it include leaves?(For example, cacti do not have leaves.)
         * 是否包含树叶（例如仙人掌不包含树叶）
         */
        bool hasLeaves = true;

    public:
        StructureInfo Generate(TileVector2D startPosition, StructureResource *structureResource) override;


        std::string GetStructureGeneratorId() override;
    };
}


#endif //GLIMMERWORKS_TREESTRUCTUREGENERATOR_H
