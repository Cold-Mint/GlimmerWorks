//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#define GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#include "../GameSystem.h"

namespace glimmer {
    class WorldPositionSystem : public GameSystem {
    public:
        void Update(float delta) override;
    };
}

#endif //GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
