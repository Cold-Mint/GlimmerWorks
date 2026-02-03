//
// Created by coldmint on 2025/12/22.
//

#ifndef GLIMMERWORKS_AUTOPICKSYSTEM_H
#define GLIMMERWORKS_AUTOPICKSYSTEM_H
#include "../GameSystem.h"
#include "../component/ItemContainerComonent.h"

namespace glimmer {
    class AutoPickSystem : public GameSystem {
    public:
        explicit AutoPickSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_AUTOPICKSYSTEM_H
