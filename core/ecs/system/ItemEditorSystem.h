//
// Created by Cold-Mint on 2026/1/23.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class ItemEditorSystem : public GameSystem {
        [[nodiscard]] bool ShouldActivate() override;

    public:
        explicit ItemEditorSystem(WorldContext *worldContext);

        bool HandleEvent(const SDL_Event &event) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}
