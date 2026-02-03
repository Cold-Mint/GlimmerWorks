//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_MAGNETSYSTEM_H
#define GLIMMERWORKS_MAGNETSYSTEM_H
#include "../GameSystem.h"


namespace glimmer {
    class MagnetSystem : public GameSystem {
    public:
        explicit MagnetSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_MAGNETSYSTEM_H
