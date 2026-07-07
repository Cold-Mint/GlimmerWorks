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

#include <cstdint>
#include <functional>
#include <memory>

#include "core/inventory/ComposableItem.h"
#include "core/inventory/Item.h"

namespace glimmer
{
    class WorldContext;
    class AudioResourceResult;
    class AudioManager;
    struct ResourceRef;

    /**
     * PlayerContext
     * 玩家上下文，负责玩家初始化、物品回调处理和物品破坏效果。
     * 从 WorldContext 拆分而来。
     */
    class PlayerContext
    {
        std::shared_ptr<std::function<void(uint8_t, Item*, ContainerChangeType)>> itemCallback_;
        std::shared_ptr<AudioResourceResult> itemBreakSFXResult_ = nullptr;
        AudioManager* audioManager_ = nullptr;
        WorldContext* worldContext_ = nullptr;

        uint32_t CreateOrLoadPlayer(const ResourceRef& resourceRef);

        void InitPlayerInventory(uint32_t playerEntity);

        void OnPlayerItemChanged(uint8_t index, Item* item, ContainerChangeType changeType, uint32_t playerEntity);

        void HandleItemBreak(Item* item, uint32_t playerEntity);

        void DropComposableItemAbilities(ComposableItem* composableItem);

    public:
        explicit PlayerContext(WorldContext* worldContext);

        ~PlayerContext();

        /**
     * Initialize the player
     * 初始化玩家
     * This method will load the player data from the disk and then supplement the player's components after the loading process.
     * 这个方法将从磁盘加载玩家数据，并在加载后补充玩家的组件。
     */
        void InitPlayer(const ResourceRef& resourceRef);

        /**
    * Initialize the hotbar
    * 初始化快捷栏
    */
        void InitHotbar(ItemContainer* itemContainer) const;

        /**
         * Initialize inventory view(Bag)
         * 初始化库存视图（背包）
         */
        void InitInventory(ItemContainer* itemContainer) const;
    };
}
