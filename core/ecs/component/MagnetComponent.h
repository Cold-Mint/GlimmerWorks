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

#include "core/ecs/GameComponent.h"
#include "core/ecs/GameEntity.h"
#include "core/Constants.h"
#include "core/ecs/EcsTypes.h"

namespace glimmer
{
    /**
     * Magnet (the side that generates suction)
     * 吸铁石（产生吸力的一方）
     */
    class MagnetComponent : public GameComponent
    {
        /**
         * radius(Add it to entities when the radius is less than or equal to this)
         * 吸附半径(当小于等于此半径时将其加入到entities内)
         */
        float adsorptionRadius_ = TILE_SIZE;

        /**
         * Detection radius (the sensing range of the magnet. When adsorbable substances enter the range, they will move towards the magnet)
         * 检测半径（吸铁石的感知范围，可吸附物进入范围时会向吸铁石移动）
         */
        float detectionRadius_ = TILE_SIZE * 5;

        /**
         * Types of magnetic adsorbates that can be adsorbed(Multiple can be set.)
         * magnetic_type_item | magnetic_type_money
         * 可被吸附的磁吸物类型(可以设置多个。)
         * 例如：MAGNETIC_TYPE_ITEM|MAGNETIC_TYPE_MONEY
         */
        uint16_t type_ = 0;

        /**
         *The id of the adsorbed entity
         * 被吸附的实体id
         */
        std::vector<GameEntityID> entities_;

    public:
        void SetType(uint16_t type);

        [[nodiscard]] uint16_t GetType() const;

        void SetDetectionRadius(float detectionRadius);

        [[nodiscard]] float GetDetectionRadius() const;

        void SetAdsorptionRadius(float adsorptionRadius);

        [[nodiscard]] float GetAdsorptionRadius() const;

        void RemoveEntity(GameEntityID entityId);

        [[nodiscard]] bool ContainEntity(GameEntityID entityId) const;

        void AddEntity(GameEntityID entityId);

        [[nodiscard]] const std::vector<GameEntityID>& GetEntities() const;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
