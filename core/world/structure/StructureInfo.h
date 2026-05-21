//
// Created by Cold-Mint on 2026/2/7.
//

#pragma once

#include "core/ecs/component/TileLayerComponent.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class StructureInfo {
        std::unordered_map<TileLayerType, std::unordered_map<TileVector2D, ResourceRef, Vector2DIHash> > structureMap_;
        bool first_ = true;
        TileVector2D minPosition_ = TileVector2D(0, 0);
        TileVector2D maxPosition_ = TileVector2D(0, 0);

    public:
        void SetTile(TileLayerType tileLayer,TileVector2D position, ResourceRef resourceRef);

        const std::unordered_map<TileLayerType, std::unordered_map<TileVector2D, ResourceRef, Vector2DIHash> > &GetStructureMap() const;
        
        [[nodiscard]] uint32_t GetWidth() const;

        [[nodiscard]] uint32_t GetHeight() const;
    };
}
