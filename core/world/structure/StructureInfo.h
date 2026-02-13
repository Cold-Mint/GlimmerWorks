//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_STRUCTUREINFO_H
#define GLIMMERWORKS_STRUCTUREINFO_H

#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class StructureInfo {
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        TileVector2D startPosition_;
        std::vector<ResourceRef> tileData_;

    public:
        StructureInfo(uint32_t width, std::vector<ResourceRef> tileData, TileVector2D startPosition);

        [[nodiscard]] uint32_t GetWidth() const;

        [[nodiscard]] uint32_t GetHeight() const;

        [[nodiscard]] TileVector2D GetStartPosition() const;
    };
}

#endif //GLIMMERWORKS_STRUCTUREINFO_H
