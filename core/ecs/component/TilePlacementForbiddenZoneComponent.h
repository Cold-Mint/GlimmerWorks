//
// Created by coldmint on 2026/4/2.
//

#pragma once
#include "core/ecs/GameComponent.h"

namespace glimmer {
    /**
     * Area where tiles are prohibited from being placed.
     * 禁止放置瓦片的区域。
     */
    class TilePlacementForbiddenZoneComponent : public GameComponent {
        int width_ = 0;
        int height_ = 0;
        int offsetX_ = 0;
        int offsetY_ = 0;

    public:
        void SetWidth(int width);

        void SetHeight(int height);

        void SetOffsetX(int offsetX);

        void SetOffsetY(int offsetY);

        [[nodiscard]] int GetWidth() const;

        [[nodiscard]] int GetHeight() const;

        [[nodiscard]] int GetOffsetX() const;

        [[nodiscard]] int GetOffsetY() const;

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
