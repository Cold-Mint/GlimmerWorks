//
// Created by Cold-Mint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGCOMPONENT_H
#define GLIMMERWORKS_DIGGINGCOMPONENT_H

#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "core/inventory/ability/MiningRangeData.h"
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

        MiningRangeData *miningRangeData_ = nullptr;
        //Explore the origin.
        //挖掘原点。
        WorldVector2D startPosition_;
        float progress_ = 0.0F;
        TileLayerType layerType_ = TileLayerType::Main;
        float efficiency_ = 1.0F;
        //precisionMining
        //精准采集
        bool precisionMining_ = false;
        //Chain collection radius
        //连锁采集半径
        int chainMiningRadius_ = 0;

    public:
        [[nodiscard]] bool IsEnable() const;

        void SetEnable(bool enable);

        void SetMiningRangeData(MiningRangeData *miningRangeData);

        [[nodiscard]] const MiningRangeData *GetMiningRangeData() const;

        void SetStartPosition(WorldVector2D startPosition);

        [[nodiscard]] const WorldVector2D &GetStartPosition() const;

        [[nodiscard]] float GetProgress() const;

        void SetLayerType(TileLayerType tileLayerType);

        [[nodiscard]] TileLayerType GetLayerType() const;

        void SetChainMiningRadius(int chainMiningRadius);

        [[nodiscard]] int GetChainMiningRadius() const;

        void SetProgress(float progress);

        void SetPrecisionMining(bool precisionMining);

        [[nodiscard]] bool IsPrecisionMining() const;

        void AddProgress(float progress);

        void SetEfficiency(float efficiency);

        [[nodiscard]] float GetEfficiency() const;

        void MarkActive();

        [[nodiscard]] bool CheckAndResetActive();

        [[nodiscard]] uint32_t GetId() override;
    };
}

#endif //GLIMMERWORKS_DIGGINGCOMPONENT_H
