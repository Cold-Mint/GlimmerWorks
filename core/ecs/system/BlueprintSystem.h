//
// Created by coldmint on 2026/5/22.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/PreloadColors.h"

namespace glimmer {
    class AppContext;

    class BlueprintSystem final : public GameSystem {
        AppContext *appContext_ = nullptr;
        TileLayerComponent *tileLayerComponent_ = nullptr;
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;
        PreloadColors *preloadColors_ = nullptr;
        PlayerComponent *playerComponent_ = nullptr;
        Transform2DComponent *playerTransform2DComponent_ = nullptr;

    public:
        explicit BlueprintSystem(WorldContext *worldContext);

        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;
    };
}
