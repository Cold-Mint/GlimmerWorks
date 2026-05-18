//
// Created by coldmint on 2026/4/2.
//

#ifndef GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
#define GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
#include "core/ecs/GameComponent.h"
#include "core/math/Vector2D.h"

namespace glimmer {
    /**
     * Area where tiles are prohibited from being placed.
     * 禁止放置瓦片的区域。
     */
    class TilePlacementForbiddenZoneComponent : public GameComponent {
        float width_ = 0;
        float height_ = 0;
        Vector2D offset_;

    public:
        void SetWidth(float width);

        void SetOffsetX(float offsetX);

        void SetOffsetY(float offsetY);

        [[nodiscard]] uint32_t GetId() override;

        [[nodiscard]] float GetOffsetX() const;

        [[nodiscard]] float GetOffsetY() const;

        void SetHeight(float height);

        [[nodiscard]] float GetWidth() const;

        [[nodiscard]] float GetHeight() const;
    };
}

#endif //GLIMMERWORKS_TILEPLACEMENTFORBIDDENZONECOMPONENT_H
