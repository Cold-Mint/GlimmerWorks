//
// Created by coldmint on 2026/2/7.
//

#include "StructureInfo.h"


glimmer::StructureInfo::StructureInfo(TileVector2D startPosition) : startPosition_(startPosition) {
    airRef_.SetSelfPackageId(RESOURCE_REF_CORE);
    airRef_.SetResourceType(RESOURCE_TYPE_TILE);
    airRef_.SetResourceKey(TILE_ID_AIR);
}

uint32_t glimmer::StructureInfo::GetWidth() const {
    return width_;
}

uint32_t glimmer::StructureInfo::GetHeight() const {
    return height_;
}

void glimmer::StructureInfo::SetTileData(uint32_t width, const std::vector<ResourceRef> &tileData) {
    tileData_ = tileData;
    width_ = width;
    height_ = tileData.size() / width;
}

void glimmer::StructureInfo::SetTile(int x, int y, const ResourceRef &resourceRef) {
    int minX = originX_;
    int minY = originY_;
    int maxX = originX_ + static_cast<int>(width_) - 1;
    int maxY = originY_ + static_cast<int>(height_) - 1;

    if (width_ == 0 || height_ == 0) {
        minX = maxX = x;
        minY = maxY = y;
    } else {
        if (x < minX) minX = x;
        if (y < minY) minY = y;
        if (x > maxX) maxX = x;
        if (y > maxY) maxY = y;
    }

    uint32_t newWidth = maxX - minX + 1;
    uint32_t newHeight = maxY - minY + 1;

    if (newWidth != width_ || newHeight != height_ || minX != originX_ || minY != originY_) {
        std::vector newData(
            newWidth * newHeight,
            airRef_);

        for (uint32_t oldY = 0; oldY < height_; ++oldY) {
            for (uint32_t oldX = 0; oldX < width_; ++oldX) {
                int worldX = originX_ + oldX;
                int worldY = originY_ + oldY;

                uint32_t newLocalX = worldX - minX;
                uint32_t newLocalY = worldY - minY;

                newData[newLocalY * newWidth + newLocalX] =
                        tileData_[oldY * width_ + oldX];
            }
        }

        tileData_ = std::move(newData);
        width_ = newWidth;
        height_ = newHeight;
        originX_ = minX;
        originY_ = minY;
    }

    uint32_t localX = x - originX_;
    uint32_t localY = y - originY_;
    tileData_[localY * width_ + localX] = resourceRef;
}


const glimmer::ResourceRef *glimmer::StructureInfo::GetResourceRef(uint32_t x, uint32_t y) const {
    if (x >= width_ || y >= height_) {
        return nullptr;
    }
    return &tileData_[x + y * width_];
}

TileVector2D glimmer::StructureInfo::GetStartPosition() const {
    return startPosition_;
}
