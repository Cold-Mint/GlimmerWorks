//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_DEBUGDRAWSYSTEM_H
#define GLIMMERWORKS_DEBUGDRAWSYSTEM_H
#include "../GameSystem.h"
#include "../component/DebugDrawComponent.h"
#include "../component/WorldPositionComponent.h"


namespace glimmer
{
    class DebugDrawSystem : public GameSystem
    {
    public:
        explicit DebugDrawSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<DebugDrawComponent>();
            RequireComponent<WorldPositionComponent>();
        }

        void Render(SDL_Renderer* renderer) override;


        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_DEBUGDRAWSYSTEM_H
