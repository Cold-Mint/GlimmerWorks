//
// Created by coldmint on 2026/2/2.
//

#ifndef GLIMMERWORKS_BOX2DSYSTEMCONTEXT_H
#define GLIMMERWORKS_BOX2DSYSTEMCONTEXT_H
#include "SDL3/SDL_render.h"

namespace glimmer {
    class WorldContext;

    class Box2dSystemContext {
        WorldContext *worldContext_ = nullptr;
        SDL_Renderer *renderer_ = nullptr;

    public:
        Box2dSystemContext(WorldContext *worldContext, SDL_Renderer *renderer);

        [[nodiscard]] WorldContext *GetWorldContext() const;

        [[nodiscard]] SDL_Renderer *GetRenderer() const;
    };
}

#endif //GLIMMERWORKS_BOX2DSYSTEMCONTEXT_H
