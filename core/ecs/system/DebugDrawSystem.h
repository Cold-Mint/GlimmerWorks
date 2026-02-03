//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_DEBUGDRAWSYSTEM_H
#define GLIMMERWORKS_DEBUGDRAWSYSTEM_H
#include "../GameSystem.h"
#include "../component/Transform2DComponent.h"


namespace glimmer {
    class DebugDrawSystem : public GameSystem {
    public:
        explicit DebugDrawSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_DEBUGDRAWSYSTEM_H
