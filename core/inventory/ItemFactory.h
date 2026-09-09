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
#include <memory>

class ItemMessage;

namespace glimmer {
    class AppContext;
    class ResourceLocator;
    class WorldContext;
    class Item;
    struct ItemMessageResource;

    /**
     * ItemFactory
     * 物品工厂
     * Creates concrete Item instances (Tile / Composable / Ability / Material) from item messages
     * by resolving resources through the resource locator.
     * 通过资源定位器解析资源，从物品消息创建具体 Item 实例（瓦片/可组合/能力/材料）。
     */
    class ItemFactory {
        AppContext *appContext_;
        ResourceLocator *resourceLocator_;

    public:
        /**
         * ItemFactory
         * 物品工厂构造
         * @param appContext appContext 应用上下文
         * @param resourceLocator resourceLocator 资源定位器
         */
        ItemFactory(AppContext *appContext, ResourceLocator *resourceLocator);

        /**
         * CreateItem
         * 从物品消息创建物品
         * @param worldContext worldContext 世界上下文
         * @param itemMessage itemMessage 物品消息
         * @return The created item, or nullptr on failure 创建的物品，失败时返回nullptr
         */
        [[nodiscard]] std::unique_ptr<Item>
        CreateItem(WorldContext *worldContext, const ItemMessage &itemMessage) const;

        /**
         * CreateItem
         * 从物品消息资源创建物品
         * @param worldContext worldContext 世界上下文
         * @param itemMessageResource itemMessageResource 物品消息资源
         * @return The created item, or nullptr on failure 创建的物品，失败时返回nullptr
         */
        [[nodiscard]] std::unique_ptr<Item> CreateItem(WorldContext *worldContext,
                                                       const ItemMessageResource &itemMessageResource) const;
    };
}
