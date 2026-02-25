//
// Created by coldmint on 2026/2/7.
//

#include "StructureInfo.h"

#include <utility>

void glimmer::StructureInfo::SetTile(const TileVector2D position, ResourceRef resourceRef) {
    structureMap_[position] = std::move(resourceRef);
    if (first_) {
        minPosition_ = position;
        maxPosition_ = position;
        first_ = false;
        return;
    }
    if (position.x < minPosition_.x) {
        minPosition_.x = position.x;
    }
    if (position.y < minPosition_.y) {
        minPosition_.y = position.y;
    }
    if (position.x > maxPosition_.x) {
        maxPosition_.x = position.x;
    }
    if (position.y > maxPosition_.y) {
        maxPosition_.y = position.y;
    }
}

const std::unordered_map<TileVector2D, glimmer::ResourceRef, glimmer::Vector2DIHash> &glimmer::StructureInfo::
GetStructureMap() const {
    return structureMap_;
}

const TileVector2D &glimmer::StructureInfo::GetMinPosition() const {
    return minPosition_;
}

uint32_t glimmer::StructureInfo::GetWidth() const {
    return maxPosition_.x - minPosition_.x + 1;
}

uint32_t glimmer::StructureInfo::GetHeight() const {
    return maxPosition_.y - minPosition_.y + 1;
}
