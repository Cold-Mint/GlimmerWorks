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
#include "CoordinateTransformer.h"

#include "core/Constants.h"

namespace glimmer
{
    SDL_FRect CoordinateTransformer::GetViewportRect(const WorldVector2D& cameraPosition,
                                                     const ScreenVector2D& cameraSize,
                                                     float zoom)
    {
        const float scaledWidth = cameraSize.x / zoom;
        const float scaledHeight = cameraSize.y / zoom;
        return {
            cameraPosition.x - scaledWidth * 0.5F,
            cameraPosition.y - scaledHeight * 0.5F,
            scaledWidth,
            scaledHeight
        };
    }

    ScreenVector2D CoordinateTransformer::WorldToScreen(const WorldVector2D& cameraPosition,
                                                        const WorldVector2D& worldPosition,
                                                        const ScreenVector2D& cameraSize,
                                                        float zoom)
    {
        const float offsetX = (worldPosition.x - cameraPosition.x) * zoom;
        const float offsetY = (worldPosition.y - cameraPosition.y) * zoom;
        return {
            cameraSize.x * 0.5F + offsetX,
            cameraSize.y * 0.5F - offsetY
        };
    }

    WorldVector2D CoordinateTransformer::ScreenToWorld(const WorldVector2D& cameraPosition,
                                                       const ScreenVector2D& screenPosition,
                                                       const ScreenVector2D& cameraSize,
                                                       float zoom)
    {
        return {
            cameraPosition.x + (screenPosition.x - cameraSize.x * 0.5F) / zoom,
            cameraPosition.y + (cameraSize.y * 0.5F - screenPosition.y) / zoom
        };
    }

    WorldVector2D CoordinateTransformer::TileToWorld(const TileVector2D& tilePos)
    {
        return {
            static_cast<float>(tilePos.x) * TILE_SIZE,
            static_cast<float>(tilePos.y) * TILE_SIZE
        };
    }

    TileVector2D CoordinateTransformer::WorldToTile(const WorldVector2D& worldPos)
    {
        return {
            static_cast<int>(std::floor(worldPos.x / TILE_SIZE + 0.5F)),
            static_cast<int>(std::floor(worldPos.y / TILE_SIZE + 0.5F))
        };
    }

    ScreenVector2D CoordinateTransformer::DesignToScreen(const DesignVector2D& designPos, float uiScale)
    {
        return {
            designPos.x * uiScale,
            designPos.y * uiScale
        };
    }
}
