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
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/BlueprintComponent.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/PreloadColors.h"

namespace glimmer
{
    class AppContext;

    class BlueprintSystem final : public GameSystem
    {
        TileLayerComponent* tileLayerComponent_ = nullptr;
        CameraComponent* cameraComponent_ = nullptr;
        Transform2DComponent* cameraTransform2DComponent_ = nullptr;
        PreloadColors* preloadColors_ = nullptr;
        BlueprintComponent* blueprintComponent_ = nullptr;
        GameEntityID player = GAME_ENTITY_ID_INVALID;
        const Tile* heldTile_ = nullptr;
        std::vector<SDL_Rect> blockRects_;
        std::vector<GameEntityID> tileLayerEntities_;
        std::vector<GameEntityID> entities_;
        uint32_t tilePlacementForbiddenZoneCount_ = 0;
        uint32_t transform2DCount_ = 0;

        /**
         * 检查矩形区域放置的可行性
         * @param tile
         * @param leftBottom
         * @param playerPosition
         * @param tileWidth
         * @param tileHeight
         * @return
         */
        std::vector<bool> CheckRectPlacementValidity(const Tile* tile, TileVector2D leftBottom,
                                                     WorldVector2D playerPosition, uint8_t tileWidth,
                                                     uint8_t tileHeight) const;

    public:
        explicit BlueprintSystem(WorldContext* worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer* renderer) override;

        [[nodiscard]] GameSystemType GetGameSystemType() override;
    };
}
