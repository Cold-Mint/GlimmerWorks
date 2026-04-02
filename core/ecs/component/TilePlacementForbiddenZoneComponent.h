//
// Created by coldmint on 2026/4/2.
//

#ifndef GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
#define GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
#include "core/ecs/GameComponent.h"

namespace glimmer {
    /**
     * Area where tiles are prohibited from being placed.
     * 禁止放置瓦片的区域。
     */
    class TilePlacementForbiddenZoneComponent : public GameComponent {
        int width_ = 0;
        int height_ = 0;
        float offsetX_ = 0;
        float offsetY_ = 0;

    public:
        void SetWidth(int width);

        void SetOffsetX(float offsetX);

        void SetOffsetY(float offsetY);

        [[nodiscard]] uint32_t GetId() override;

        [[nodiscard]] float GetOffsetX() const;

        [[nodiscard]] float GetOffsetY() const;

        void SetHeight(int height);

        [[nodiscard]] int GetWidth() const;

        [[nodiscard]] int GetHeight() const;
    };
}

#endif //GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
