//
// Created by coldmint on 2026/3/3.
//

#ifndef GLIMMERWORKS_SPIRITRENDERERSYSTEM_H
#define GLIMMERWORKS_SPIRITRENDERERSYSTEM_H

#include "core/ecs/GameSystem.h"
#include "core/mod/ResourceRef.h"

namespace glimmer {
    class SpiritRendererSystem final : public GameSystem {
    public:
        explicit SpiritRendererSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_SPIRITRENDERERSYSTEM_H
