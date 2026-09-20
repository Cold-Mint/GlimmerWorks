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
#include "Dimension.h"

#include <cmath>

#include "core/config/Constants.h"
#include "core/mod/Resource.h"


void glimmer::Dimension::SetDimensionResource(DimensionResource *dimensionResource) {
    dimensionResource_ = dimensionResource;
    if (dimensionResource_ != nullptr) {
        normalizedTime_.store(Normalize(dimensionResource_->initialTime));
    }
    lastTick_ = 0;
    initedTick_ = false;
}

glimmer::DimensionResource *glimmer::Dimension::GetDimensionResource() const {
    return dimensionResource_;
}

void glimmer::Dimension::AdvanceTime(const uint64_t tick) {
    if (!initedTick_) {
        lastTick_ = tick;
        initedTick_ = true;
        return;
    }
    const uint64_t tickDelta = tick - lastTick_;
    lastTick_ = tick;
    if (tickDelta == 0) {
        return;
    }
    const DimensionResource *dimensionResource = dimensionResource_;
    if (dimensionResource == nullptr || dimensionResource->timeFlowSpeed <= 0.0F) {
        return;
    }
    const DayNormalizedTime advanced = normalizedTime_.load(std::memory_order_relaxed) +
                                       dimensionResource->timeFlowSpeed * static_cast<DayNormalizedTime>(tickDelta) /
                                       static_cast<DayNormalizedTime>(DAY_LENGTH);
    normalizedTime_.store(Normalize(advanced), std::memory_order_relaxed);
}

glimmer::DayNormalizedTime glimmer::Dimension::GetNormalizedTime() const {
    return normalizedTime_.load(std::memory_order_relaxed);
}

void glimmer::Dimension::SetNormalizedTime(const DayNormalizedTime normalizedTime) {
    normalizedTime_.store(Normalize(normalizedTime), std::memory_order_relaxed);
}

glimmer::DayNormalizedTime glimmer::Dimension::Normalize(DayNormalizedTime time) {
    time = std::fmod(time, 1.0F);
    if (time < 0.0F) {
        time += 1.0F;
    }
    return time;
}
