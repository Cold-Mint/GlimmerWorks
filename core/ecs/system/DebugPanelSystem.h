//
// Created by Cold-Mint on 2025/11/3.
//

#ifndef GLIMMERWORKS_DEBUGPANELSYSTEM_H
#define GLIMMERWORKS_DEBUGPANELSYSTEM_H
#include "../GameSystem.h"

namespace glimmer {
    class DebugPanelSystem : public GameSystem {
        WorldVector2D mousePosition_ = WorldVector2D{};

        [[nodiscard]] bool ShouldActivate() override;

        void RenderDebugText(SDL_Renderer *renderer, int windowW, const char *text, float y) const;

    public:
        DebugPanelSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
        }

        void Render(SDL_Renderer *renderer) override;

        bool HandleEvent(const SDL_Event &event) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DEBUGPANELSYSTEM_H
