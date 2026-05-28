//
// Created by Cold-Mint on 2026/5/24.
//

#include "MiningRangeDataPoint.h"

const TileVector2D &glimmer::MiningRangeDataPoint::GetTileTopLeftPosition() const {
    return tileTopLeftPosition_;
}

uint8_t glimmer::MiningRangeDataPoint::GetWidth() const {
    return width_;
}

uint8_t glimmer::MiningRangeDataPoint::GetHeight() const {
    return height_;
}

void glimmer::MiningRangeDataPoint::SetWidth(const uint8_t width) {
    width_ = width;
}

void glimmer::MiningRangeDataPoint::SetHeight(const uint8_t height) {
    height_ = height;
}

void glimmer::MiningRangeDataPoint::SetTileTopLeftPosition(const TileVector2D &tileTopLeftPosition) {
    tileTopLeftPosition_ = tileTopLeftPosition;
}
