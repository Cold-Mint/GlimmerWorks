//
// Created by Cold-Mint on 2025/12/18.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/ItemContainerComonent.h"

namespace glimmer {
    /**
     * HotBar
     * 物品快捷栏
     */
    class HotBarSystem : public GameSystem {
    public:
        explicit HotBarSystem(WorldContext *worldContext);

        bool HandleEvent(const SDL_Event &event) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}
