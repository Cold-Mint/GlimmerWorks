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
#include "core/math/TileVector2D.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer {
    class Tile;
    /**
     * CropComponent
     * 作物组件
     *
     * Attached to the entity of a placed crop tile. CropSystem reads this
     * component every tick, evaluates the tile's growth conditions and
     * accumulates growth ticks when they are satisfied.
     * 挂载到已放置的作物瓦片实体上。CropSystem 每 tick 读取该组件，
     * 判断瓦片的生长条件，并在条件满足时累积生长 tick。
     */
    class CropComponent : public GameComponent {
        TileVector2D position_;
        TileLayerType layerType_ = TileLayerType::Ground;

    public:
        void SetPosition(const TileVector2D &position);

        [[nodiscard]] const TileVector2D &GetPosition() const;

        void SetLayerType(TileLayerType layerType);

        [[nodiscard]] TileLayerType GetLayerType() const;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
