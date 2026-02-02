//
// Created by Cold-Mint on 2026/1/8.
//

#ifndef GLIMMERWORKS_PAUSESYSTEM_H
#define GLIMMERWORKS_PAUSESYSTEM_H
#include "../GameSystem.h"

namespace glimmer {
    class PauseComponent;

    class PauseSystem : public GameSystem {
    public:
        PauseSystem(AppContext *appContext, WorldContext *worldContext);


        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;


        bool OnBackPressed() override;

        [[nodiscard]] bool CanRunWhilePaused() const override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_PAUSESYSTEM_H
