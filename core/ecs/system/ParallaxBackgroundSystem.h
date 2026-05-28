//
// Created by Cold-Mint on 2026/5/10.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class ParallaxBackgroundSystem : public GameSystem {
    public:
        explicit ParallaxBackgroundSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}
