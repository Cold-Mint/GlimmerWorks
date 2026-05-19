//
// Created by Cold-Mint on 2025/10/29.
//

#pragma once
#include "core/ecs/GameSystem.h"

namespace glimmer {
    /**
     * The camera system handles the logic of the camera and its interaction with the world coordinates
     * 相机系统，处理相机的逻辑和与世界坐标的联动
     */
    class CameraSystem final : public GameSystem {
    public:
        explicit CameraSystem(WorldContext *worldContext);

        void Render(SDL_Renderer *renderer) override;


        std::string GetName() override;
    };
}
