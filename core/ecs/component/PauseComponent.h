//
// Created by Cold-Mint on 2026/1/8.
//

#pragma once
#include "core/ecs/GameComponent.h"

namespace glimmer {
    class PauseComponent : public GameComponent {
    public:
        [[nodiscard]] uint32_t GetId() override;
    };
}
