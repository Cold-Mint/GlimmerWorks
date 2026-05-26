//
// Created by Cold-Mint on 2025/11/2.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/TileLayerComponent.h"

namespace glimmer
{
    enum TileLayerType : uint8_t;

    class TileLayerSystem final : public GameSystem
    {
    public:
        explicit TileLayerSystem(WorldContext* worldContext);

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;

        bool HandleEvent(const SDL_Event& event) override;

        std::string GetName() override;
    };
}
