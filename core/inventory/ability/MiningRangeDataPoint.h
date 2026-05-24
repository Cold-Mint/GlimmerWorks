//
// Created by coldmint on 2026/5/24.
//

#pragma once
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    class MiningRangeDataPoint {
        TileVector2D tileTopLeftPosition_;
        uint8_t width_ = 1;
        uint8_t height_ = 1;

    public:
        [[nodiscard]] const TileVector2D &GetTileTopLeftPosition() const;

        [[nodiscard]] uint8_t GetWidth() const;

        [[nodiscard]] uint8_t GetHeight() const;

        void SetWidth(uint8_t width);

        void SetHeight(uint8_t height);

        void SetTileTopLeftPosition(const TileVector2D &tileTopLeftPosition);
    };
}
