//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#define GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#include "../GameSystem.h"

namespace glimmer {
    class Transform2DSystem final : public GameSystem {
    public:
        explicit Transform2DSystem(WorldContext *worldContext);

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
