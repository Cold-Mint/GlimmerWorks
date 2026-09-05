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

#include <vector>

#include "WeatherConditionProcessorManager.h"

namespace glimmer {
    class WorldContext;
    class Dimension;
    struct SkyElementResource;
    struct WeatherResource;

    /**
     * WeatherManager
     * 天气管理器（每个维度一个）
     * Drives the generic weather intensity axis and matches weathers to collect sky elements.
     * 驱动通用天气强度轴，并匹配天气以收集天空元素。
     */
    class WeatherManager {
        WorldContext *worldContext_ = nullptr;
        Dimension *dimension_ = nullptr;
        WeatherConditionProcessorManager conditionProcessorManager_;

        /**
         * Generic weather intensity (0..1).
         * 通用天气强度（0..1）。
         */
        float weatherIntensity_ = 0.0F;

        /**
         * Remaining seconds until the next intensity change.
         * 距下一次强度变化的剩余秒数。
         */
        float weatherTimer_ = 0.0F;

        [[nodiscard]] float PickNextIntensity() const;

        [[nodiscard]] float PickNextDuration() const;

        [[nodiscard]] bool MatchesWeather(const WeatherContext &ctx, const WeatherResource *weather) const;

    public:
        WeatherManager(WorldContext *worldContext, Dimension *dimension);

        /**
         * Update
         * 推进天气强度轴（游戏暂停时不推进）。
         * @param delta delta 上一帧耗时（秒）
         */
        void Update(float delta);

        [[nodiscard]] float GetWeatherIntensity() const;

        [[nodiscard]] float GetWeatherTimer() const;

        void SetWeatherIntensity(float intensity);

        void SetWeatherTimer(float timer);

        /**
         * GetActiveElements
         * 求值全部天气，收集所有命中天气的天空元素，并按 (slot, animationType) 去冲突。
         * @return 最终生效的天空元素列表（按 slot 排序）
         */
        [[nodiscard]] std::vector<const SkyElementResource *> GetActiveElements();
    };
}
