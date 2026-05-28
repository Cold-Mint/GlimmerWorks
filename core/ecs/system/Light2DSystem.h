//
// Created by Cold-Mint on 2026/4/10.
//


#pragma once
#include "core/ecs/GameSystem.h"
#include "core/world/Tile.h"


namespace glimmer {
    class Light2DSystem : public GameSystem {
    public:
        explicit Light2DSystem(WorldContext *worldContext);

        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;
    };
}
