//
// Created by Cold-Mint on 2026/2/26.
//

#pragma once
#include "TileLayerComponent.h"
#include "core/ecs/GameComponent.h"

namespace glimmer {
    class AreaMarkerComponent : public GameComponent {
        bool first_ = true;
        TileVector2D startPoint_;
        TileVector2D endPoint_;
        float remainingTime_ = 0;
        float MAX_REMAINING_TIME = 0.05F;

    public:
        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;

        [[nodiscard]] float GetRemainingTime() const;

        void SetRemainingTime(float remainingTime);

        [[nodiscard]] bool IsExpired() const;

        /**
         * Is it necessary to draw on the page?
         * 是否需要绘制在页面上
         * @return
         */
        [[nodiscard]] bool CanDraw() const;

        void Reset();

        void SetPoint(TileVector2D point);

        [[nodiscard]] const TileVector2D &GetStartPoint() const;

        [[nodiscard]] const TileVector2D &GetEndPoint() const;
    };
}
