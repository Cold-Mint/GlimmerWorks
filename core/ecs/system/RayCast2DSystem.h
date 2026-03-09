//
// Created by coldmint on 2026/3/8.
//

#ifndef GLIMMERWORKS_RAYCAST2DSYSTEM_H
#define GLIMMERWORKS_RAYCAST2DSYSTEM_H
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class RayCast2DSystem final : public GameSystem {
    public:
        explicit RayCast2DSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_RAYCAST2DSYSTEM_H
