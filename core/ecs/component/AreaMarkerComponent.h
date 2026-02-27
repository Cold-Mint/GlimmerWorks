//
// Created by coldmint on 2026/2/26.
//

#ifndef GLIMMERWORKS_AREAMARKERCOMPONENT_H
#define GLIMMERWORKS_AREAMARKERCOMPONENT_H
#include "TileLayerComponent.h"
#include "core/ecs/GameComponent.h"
#include "SDL3/SDL_pixels.h"

namespace glimmer {
    class AreaMarkerComponent : public GameComponent {
        bool first_ = true;
        TileVector2D startPoint_;
        TileVector2D endPoint_;
        float remainingTime_ = 0;
        float MAX_REMAINING_TIME = 0.05F;
        SDL_Color areaMarkerBorder_ = {};
        SDL_Color areaMarkerFull_ = {};

    public:
        [[nodiscard]] uint32_t GetId() override;

        [[nodiscard]] float GetRemainingTime() const;

        void SetRemainingTime(float remainingTime);

        [[nodiscard]] bool IsExpired() const;

        [[nodiscard]] const SDL_Color &GetAreaMarkerBorderColor() const;

        [[nodiscard]] const SDL_Color &GetAreaMarkerFullColor() const;

        void SetAreaMarkerBorderColor(const SDL_Color &color);

        void SetAreaMarkerFullColor(const SDL_Color &color);

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

        [[nodiscard]] bool IsSerializable() override;
    };
}

#endif //GLIMMERWORKS_AREAMARKERCOMPONENT_H
