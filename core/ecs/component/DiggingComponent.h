//
// Created by coldmint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGCOMPONENT_H
#define GLIMMERWORKS_DIGGINGCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"

namespace glimmer {
    /**
     * DiggingComponent
     * 正在挖掘组件
     *
     * This component saves the state of the player digging blocks.
     * 此组件保存玩家挖掘方块的状态。
     */
    class DiggingComponent : public GameComponent {
        bool enable_ = false;
        WorldVector2D position_;
        float progress_ = 0.0F;

    public:
        [[nodiscard]] bool IsEnable() const;

        void SetEnable(bool enable);

        [[nodiscard]] WorldVector2D GetPosition() const;

        void SetPosition(const WorldVector2D &position);

        [[nodiscard]] float GetProgress() const;

        void SetProgress(float progress);
    };
}

#endif //GLIMMERWORKS_DIGGINGCOMPONENT_H
