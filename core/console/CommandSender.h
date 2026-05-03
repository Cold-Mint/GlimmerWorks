//
// Created by coldmint on 2026/5/3.
//

#ifndef GLIMMERWORKS_COMMANDSENDER_H
#define GLIMMERWORKS_COMMANDSENDER_H
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer {
    class CommandSender {
        WorldVector2D position_;

    public:
        void SetPosition(const WorldVector2D &position);

        [[nodiscard]] WorldVector2D GetPosition() const;
    };
}

#endif //GLIMMERWORKS_COMMANDSENDER_H
