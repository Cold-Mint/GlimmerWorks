//
// Created by Cold-Mint on 2025/10/29.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class DebugDrawSystem : public GameSystem {
    public:
        explicit DebugDrawSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}
