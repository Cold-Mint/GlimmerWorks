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
#include "core/inventory/Item.h"

namespace glimmer {
    class DroppedItemComponent : public GameComponent {
        std::unique_ptr<Item> item_ = nullptr;
        //How long will it be before it disappears
        //还剩多久会消失(秒)
        float remainingTime_ = 60.0F;
        float pickupCooldown_ = 0.0F;

    public:
        explicit DroppedItemComponent();

        /**
         * Get Remaining Time
         * 获取剩余时间
         * @return
         */
        [[nodiscard]] float GetRemainingTime() const;

        /**
         * Set Remaining Time
         * 设置剩余时间
         * @param remainingTime
         */
        void SetRemainingTime(float remainingTime);

        /**
         * Is Expired
         * 是否过期了
         * @return
         */
        [[nodiscard]] bool IsExpired() const;

        void SetItem(std::unique_ptr<Item> item);

        /**
         * ExtractItem
         * 提取物品
         * @return
         */
        [[nodiscard]] std::unique_ptr<Item> ExtractItem();

        [[nodiscard]] Item *GetItem() const;

        /**
         * Set Pickup Cooldown
         * 设置拾取冷却
         * @param cooldown
         */
        void SetPickupCooldown(float cooldown);

        /**
         * Get Pickup Cooldown
         * 获取拾取冷却
         * @return
         */
        [[nodiscard]] float GetPickupCooldown() const;

        /**
         * Can Be Picked Up
         * 是否可以被拾取
         * @return
         */
        [[nodiscard]] bool CanBePickedUp() const;

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;

        [[nodiscard]] bool IsSerializable() override;

        [[nodiscard]] std::string Serialize() override;

        void Deserialize(WorldContext *worldContext, const std::string &data) override;
    };
}
