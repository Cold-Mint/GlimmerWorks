//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_WORLDPOSITIONCOMPONENT_H
#define GLIMMERWORKS_WORLDPOSITIONCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"

namespace glimmer
{
    /**
     * The world coordinate component is used to record the absolute position of objects in the 2D game world
     * 世界坐标组件，用于记录物体在2D游戏世界中的绝对位置
     */
    class WorldPositionComponent final : public GameComponent
    {
        WorldVector2D position_;

    public:
        WorldPositionComponent() = default;

        explicit WorldPositionComponent(const WorldVector2D position) : position_(position)
        {
        }

        /**
         * SetPosition 设置世界坐标
         * @param newPosition newPosition 新的坐标
         */
        void SetPosition(WorldVector2D newPosition);

        /**
         * Translate
         * 移动坐标
         * @param deltaPosition deltaPosition 移动量
         */
        void Translate(WorldVector2D deltaPosition);

        /**
         * GetPosition 获取世界坐标
         * @return 世界坐标
         */
        [[nodiscard]] WorldVector2D GetPosition() const;
    };
}

#endif //GLIMMERWORKS_WORLDPOSITIONCOMPONENT_H
