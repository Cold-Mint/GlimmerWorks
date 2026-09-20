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
#include <cstdint>

#include "DayNormalizedTime.h"
#include "core/mod/Resource.h"

namespace glimmer {
    /**
     * Dimension
     * 维度
     * A dimension holds its own world generation, chunk storage, terrain and time state.
     * 维度持有独立的世界生成、区块存储、地形和时间状态。
     */
    class Dimension {
        DimensionResource *dimensionResource_ = nullptr;

        /**
         * The current time of day in this dimension, normalized to 0..1.
         * 此维度当前的一天中的时间，归一化到 0..1。
         * Written on the tick thread and read on the render thread, so it is
         * stored atomically. 在 tick 线程写入、渲染线程读取，故以原子类型存储。
         */
        std::atomic<DayNormalizedTime> normalizedTime_{0.0F};

        //Last tick seen by AdvanceTime; used to compute the tick delta.
        //AdvanceTime 上次看到的 tick；用于计算 tick 差值。
        uint64_t lastTick_ = 0;

        //Whether the tick baseline has been initialized.
        //tick 基线是否已初始化。
        bool initedTick_ = false;

        /**
         * Normalize
         * 归一化
         * Wraps a time value into the [0, 1) range.
         * 将时间值回绕到 [0, 1) 区间。
         */
        static DayNormalizedTime Normalize(DayNormalizedTime time);

    public:
        void SetDimensionResource(DimensionResource *dimensionResource);

        [[nodiscard]] DimensionResource *GetDimensionResource() const;

        /**
         * AdvanceTime
         * 推进时间
         * Advances the normalized day time (0..1) based on the elapsed ticks and
         * the dimension's time flow speed. Time wraps around at 1.0 back to 0.0.
         * 根据流逝的 tick 数与维度的时间流动速度推进归一化日时间（0..1）。
         * 时间在 1.0 处回绕到 0.0。
         * @param tick tick The cumulative tick counter. 累计 tick 计数。
         */
        void AdvanceTime(uint64_t tick);

        /**
         * GetNormalizedTime
         * 获取归一化时间
         * @return The current normalized day time (0..1). 当前归一化日时间（0..1）。
         */
        [[nodiscard]] DayNormalizedTime GetNormalizedTime() const;

        /**
         * SetNormalizedTime
         * 设置归一化时间
         * @param normalizedTime normalizedTime The normalized day time (0..1). 归一化日时间（0..1）。
         */
        void SetNormalizedTime(DayNormalizedTime normalizedTime);
    };
}
