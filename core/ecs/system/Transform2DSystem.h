//
// Created by Cold-Mint on 2025/10/28.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class Transform2DSystem final : public GameSystem {
    public:
        explicit Transform2DSystem(WorldContext *worldContext);

        std::string GetName() override;
    };
}
