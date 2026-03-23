//
// Created by Cold-Mint on 2026/2/26.
//

#ifndef GLIMMERWORKS_AREAMARKERSYSTEM_H
#define GLIMMERWORKS_AREAMARKERSYSTEM_H
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/AreaMarkerComponent.h"

namespace glimmer {
    class AreaMarkerSystem : public GameSystem {
    public:
        explicit AreaMarkerSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}

#endif //GLIMMERWORKS_AREAMARKERSYSTEM_H
