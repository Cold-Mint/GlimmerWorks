//
// Created by Cold-Mint on 2026/3/28.
//

#pragma once
#include "core/ecs/GameSystem.h"


namespace glimmer {
    class FloatingTextSystem : public GameSystem {
    public:
        explicit FloatingTextSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;


        std::string GetName() override;
    };
}
