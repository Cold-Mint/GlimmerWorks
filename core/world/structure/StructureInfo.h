//
// Created by coldmint on 2026/2/7.
//

#ifndef GLIMMERWORKS_STRUCTUREINFO_H
#define GLIMMERWORKS_STRUCTUREINFO_H

#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class StructureInfo {
        std::unordered_map<TileVector2D, ResourceRef, Vector2DIHash> structureMap_;
        bool first_ = true;
        TileVector2D minPosition_ = TileVector2D(0, 0);
        TileVector2D maxPosition_ = TileVector2D(0, 0);

    public:
        void SetTile(TileVector2D position, ResourceRef resourceRef);

        const std::unordered_map<TileVector2D, ResourceRef, Vector2DIHash> &GetStructureMap() const;

        const TileVector2D &GetMinPosition() const;

        [[nodiscard]] uint32_t GetWidth() const;

        [[nodiscard]] uint32_t GetHeight() const;
    };
}

#endif //GLIMMERWORKS_STRUCTUREINFO_H
