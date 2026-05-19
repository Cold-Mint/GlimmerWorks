//
// Created by Cold-Mint on 2025/12/22.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class MagnetSystem : public GameSystem {
    public:
        explicit MagnetSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}
