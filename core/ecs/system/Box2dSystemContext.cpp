//
// Created by coldmint on 2026/2/2.
//

#include "Box2dSystemContext.h"

glimmer::Box2dSystemContext::Box2dSystemContext(WorldContext *worldContext,
                                                SDL_Renderer *renderer) : worldContext_(worldContext),
                                                                          renderer_(renderer) {
}

glimmer::WorldContext *glimmer::Box2dSystemContext::GetWorldContext() const {
    return worldContext_;
}

SDL_Renderer *glimmer::Box2dSystemContext::GetRenderer() const {
    return renderer_;
}
