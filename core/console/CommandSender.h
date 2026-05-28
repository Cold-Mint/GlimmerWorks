//
// Created by Cold-Mint on 2026/5/3.
//

#pragma once
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    class CommandSender {
        WorldVector2D position_;

    public:
        void SetPosition(const WorldVector2D &position);

        [[nodiscard]] WorldVector2D GetPosition() const;
    };
}
