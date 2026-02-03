//
// Created by coldmint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGCOMPONENT_H
#define GLIMMERWORKS_DIGGINGCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "core/world/generator/TileLayerType.h"

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
        bool activeSignal_ = false;
        WorldVector2D position_;
        float progress_ = 0.0F;
        TileLayerType layerType_ = TileLayerType::Main;
        float efficiency_ = 1.0F;

    public:
        [[nodiscard]] bool IsEnable() const;

        void SetEnable(bool enable);

        [[nodiscard]] WorldVector2D GetPosition() const;

        void SetPosition(const WorldVector2D &position);

        [[nodiscard]] float GetProgress() const;

        void SetLayerType(TileLayerType tileLayerType);

        [[nodiscard]] TileLayerType GetLayerType() const;

        void SetProgress(float progress);

        void AddProgress(float progress);

        void SetEfficiency(float efficiency);

        [[nodiscard]] float GetEfficiency() const;

        void MarkActive();

        [[nodiscard]] bool CheckAndResetActive();

        [[nodiscard]] u_int32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_DIGGINGCOMPONENT_H
