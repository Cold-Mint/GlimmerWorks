//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_AUTOPICKSYSTEM_H
#define GLIMMERWORKS_AUTOPICKSYSTEM_H
#include "../GameSystem.h"
#include "../component/AutoPickComponent.h"
#include "../component/ItemContainerComonent.h"
#include "../component/MagnetComponent.h"

namespace glimmer {
    class AutoPickSystem : public GameSystem {
    public:
        AutoPickSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
            RequireComponent<AutoPickComponent>();
            RequireComponent<MagnetComponent>();
            RequireComponent<ItemContainerComponent>();
        }

        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_AUTOPICKSYSTEM_H
