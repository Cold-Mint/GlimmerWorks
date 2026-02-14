//
// Created by coldmint on 2026/2/7.
//

#include "StructureInfo.h"


glimmer::StructureInfo::StructureInfo(const uint32_t width, std::vector<ResourceRef> *tileData,
                                      const TileVector2D startPosition) : width_(width),
                                                                          height_(tileData->size() / width_),
                                                                          startPosition_(startPosition),
                                                                          tileData_(tileData) {
}

uint32_t glimmer::StructureInfo::GetWidth() const {
    return width_;
}

uint32_t glimmer::StructureInfo::GetHeight() const {
    return height_;
}

glimmer::ResourceRef *glimmer::StructureInfo::GetResourceRef(uint32_t x, uint32_t y) const {
    return &(*tileData_)[x + y * width_];
}

TileVector2D glimmer::StructureInfo::GetStartPosition() const {
    return startPosition_;
}
