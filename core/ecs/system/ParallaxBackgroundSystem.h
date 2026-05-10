//
// Created by coldmint on 2026/5/10.
//

#ifndef GLIMMERWORKS_PARALLAXBACKGROUNDSYSTEM_H
#define GLIMMERWORKS_PARALLAXBACKGROUNDSYSTEM_H
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class ParallaxBackgroundSystem : public GameSystem {
    public:
        explicit ParallaxBackgroundSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}


#endif //GLIMMERWORKS_PARALLAXBACKGROUNDSYSTEM_H
