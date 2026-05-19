//
// Created by Cold-Mint on 2026/2/26.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/AreaMarkerComponent.h"

namespace glimmer {
    class AreaMarkerSystem : public GameSystem {
    public:
        explicit AreaMarkerSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}
