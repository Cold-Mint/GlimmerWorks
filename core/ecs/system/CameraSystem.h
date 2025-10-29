//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_CAMERASYSTEM_H
#define GLIMMERWORKS_CAMERASYSTEM_H
#include "../GameSystem.h"
#include "../component/CameraComponent.h"
#include "../component/WorldPositionComponent.h"

namespace glimmer
{
    /**
     * The camera system handles the logic of the camera and its interaction with the world coordinates
     * 相机系统，处理相机的逻辑和与世界坐标的联动
     */
    class CameraSystem final : public GameSystem
    {
    public:
        explicit CameraSystem(WorldContext* worldContext)
            : GameSystem(worldContext)
        {
            RequireComponent<CameraComponent>();
            RequireComponent<WorldPositionComponent>();
        }



        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_CAMERASYSTEM_H
