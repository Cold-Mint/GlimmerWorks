//
// Created by Cold-Mint on 2025/10/26.
//

#ifndef GLIMMERWORKS_GAMESYSTEM_H
#define GLIMMERWORKS_GAMESYSTEM_H
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"

namespace glimmer {
    class GameSystem {
    public:
        virtual ~GameSystem() = default;

        virtual bool HandleEvent(const SDL_Event &event) = 0;

        virtual void Update(float delta) = 0;

        virtual void Render(SDL_Renderer *renderer) = 0;
    };
}

#endif //GLIMMERWORKS_GAMESYSTEM_H
