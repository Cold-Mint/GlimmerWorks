//
// Created by Cold-Mint on 2025/12/22.
//

#pragma once
#include "core/ecs/GameComponent.h"

namespace glimmer {
    class AutoPickComponent : public GameComponent {
    public:
        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;

        [[nodiscard]] bool IsSerializable() override;
    };
}
