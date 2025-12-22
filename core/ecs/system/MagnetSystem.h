//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_MAGNETSYSTEM_H
#define GLIMMERWORKS_MAGNETSYSTEM_H
#include "../GameSystem.h"
#include "../component/MagnetComponent.h"
#include "../component/MagneticComponent.h"
#include "../component/Transform2DComponent.h"

namespace glimmer {
    class MagnetSystem : public GameSystem {
    public:
        MagnetSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
            RequireComponent<Transform2DComponent>();
            RequireComponent<MagnetComponent>();
            RequireComponent<MagneticComponent>();
        }

        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_MAGNETSYSTEM_H
