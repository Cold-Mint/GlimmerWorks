//
// Created by Cold-Mint on 2026/1/8.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class PauseComponent;

    class PauseSystem : public GameSystem {
    public:
        explicit PauseSystem(WorldContext *worldContext);


        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;


        bool OnBackPressed() override;

        [[nodiscard]] bool CanRunWhilePaused() const override;

        std::string GetName() override;
    };
}
