/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#pragma once
#include "core/ecs/GameComponent.h"
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
        TileVector2D startPosition_;
        float progress_ = 0.0F;
        TileLayerType layerType_ = Ground;
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

        void SetStartPosition(TileVector2D startPosition);

        [[nodiscard]] const TileVector2D &GetStartPosition() const;

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

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
