//
// Created by coldmint on 2026/4/10.
//

#ifndef GLIMMERWORKS_LIGHT2DSYSTEM_H
#define GLIMMERWORKS_LIGHT2DSYSTEM_H
#include "core/ecs/GameSystem.h"
#include "core/world/Tile.h"


namespace glimmer {
    class Light2DSystem : public GameSystem {
    public:
        explicit Light2DSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_LIGHT2DSYSTEM_H
