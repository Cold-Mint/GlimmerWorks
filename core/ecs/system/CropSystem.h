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
#include <mutex>
#include <vector>

#include "core/ecs/GameSystem.h"

namespace glimmer {
    class CropComponent;

    /**
     * CropSystem
     * 作物系统
     *
     * Iterates over all CropComponent each tick and, using the growth condition
     * processors, decides whether each crop should grow. Crops that meet their
     * conditions accumulate ticks in their TileStateMessage.
     * 每个 tick 遍历所有作物组件，并使用生长条件处理器判断作物是否应该生长；
     * 满足条件的作物在其 TileStateMessage 中累积生长 tick。
     */
    class CropSystem final : public GameSystem {
        uint32_t cropCount_ = 0;
        std::vector<CropComponent *> cropComponents_;
        /**
         * Protects cropComponents_ (and cropCount_) against concurrent rebuild on
         * the main thread (OnWatchedComponentChanged, called from OnFrameStart)
         * and iteration on the tick thread (OnTick).
         * 保护 cropComponents_（以及 cropCount_）免受主线程
         * （OnWatchedComponentChanged，由 OnFrameStart 调用）重建
         * 与 tick 线程（OnTick）遍历的并发访问。
         */
        mutable std::mutex cropMutex_;

    public:
        explicit CropSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;


        /**
         * OnGrowMature
         * 当生长完毕后。
         * @param worldContext worldContext 世界上下文
         * @param chunk chunk 区块
         * @param position position  位置
         * @param layerType layerType  图层类型
         * @param growthTargetRef growthTargetRef 目标资源引用
         */
        static bool OnGrowMature(WorldContext *worldContext, Chunk *chunk, const TileVector2D &position,
                                 TileLayerType layerType, const ResourceRef *growthTargetRef);

        void OnTick(uint64_t tick) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
