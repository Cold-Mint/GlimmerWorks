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
#include <atomic>
#include <mutex>

#include "core/ecs/GameSystem.h"
#include "core/math/WorldVector2D.h"

namespace glimmer {
    class TechProviderSystem final : public GameSystem {
        uint32_t transform2DCount_ = 0;
        uint32_t techProviderCount_ = 0;
        std::vector<GameEntityID> techProviderEntities_;
        /**
         * Protects techProviderEntities_ against concurrent rebuild on the main
         * thread (OnWatchedComponentChanged) and iteration on the tick thread
         * (OnTick).
         * 保护 techProviderEntities_ 免受主线程（OnWatchedComponentChanged）
         * 重建与 tick 线程（OnTick）遍历的并发访问。
         */
        mutable std::mutex techProviderMutex_;
        GameEntityID player_ = 0;
        //Whether the technology map needs to be recomputed. Set on the main
        //thread (OnWatchedComponentChanged), consumed on the tick thread (OnTick).
        //是否需要重新计算科技表。在主线程（OnWatchedComponentChanged）设置，在 tick 线程（OnTick）消费。
        std::atomic<bool> changed = false;
        WorldVector2D lastPlayerPosition_{};

    public:
        explicit TechProviderSystem(WorldContext *worldContext);

        void OnActivationChanged(bool activeStatus) override;

        void OnTick(uint64_t tick) override;

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
