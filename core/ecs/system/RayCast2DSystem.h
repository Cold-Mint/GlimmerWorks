//
// Created by Cold-Mint on 2026/3/8.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class RayCast2DSystem final : public GameSystem {
    public:
        explicit RayCast2DSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}
