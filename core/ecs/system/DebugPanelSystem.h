//
// Created by Cold-Mint on 2025/11/3.
//

#ifndef GLIMMERWORKS_DEBUGPANELSYSTEM_H
#define GLIMMERWORKS_DEBUGPANELSYSTEM_H
#include "../GameSystem.h"

namespace glimmer
{
    class DebugPanelSystem : public GameSystem
    {
        [[nodiscard]] bool ShouldActivate() override;

    public:
        DebugPanelSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
        }

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DEBUGPANELSYSTEM_H
