//
// Created by Cold-Mint on 2025/12/20.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class DroppedItemSystem : public GameSystem {
    public:
        explicit DroppedItemSystem(WorldContext *worldContext);

        uint8_t GetRenderOrder() override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;
    };
}
