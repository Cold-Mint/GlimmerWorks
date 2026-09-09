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
#include "TerrainMath.h"

#include "core/config/Constants.h"

float glimmer::TerrainMath::GetElevation(const int y) {
    return static_cast<float>(y - WORLD_MIN_Y) / static_cast<float>(WORLD_MAX_Y - WORLD_MIN_Y);
}

float glimmer::TerrainMath::GetSurfaceProximity(const int firstTileTerrainY, const int worldY) {
    constexpr float totalHeight = WORLD_MAX_Y - WORLD_MIN_Y;
    const float surfaceNormalized = static_cast<float>(firstTileTerrainY - WORLD_MIN_Y) / totalHeight;
    const float currentNormalized = static_cast<float>(worldY - WORLD_MIN_Y) / totalHeight;
    const float offset = currentNormalized - surfaceNormalized;
    float proximity = 0.5F + offset * 0.5F;
    if (proximity < 0.0F) {
        proximity = 0.0F;
    }
    if (proximity > 1.0F) {
        proximity = 1.0F;
    }
    return proximity;
}
