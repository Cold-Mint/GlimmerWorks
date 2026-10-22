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
#include <unordered_set>
#include <vector>

#include "ability/AbilityType.h"
#include "core/math/TileVector2D.h"

class TileStateMessage;

namespace glimmer {
    class Item;
    class Tile;
    class TileLayerComponent;
    class WorldContext;
    /**
     * ItemUseContext
     * 使用物品时的上下文。封装使用物品所需的环境与参数，
     * 后续新增能力（例如交互）时只需扩展此类，无需再修改 OnUse 的签名。
     */
    struct ItemUseContext {
        std::vector<TileLayerComponent *> focusedTileLayerComponents;
        std::vector<const TileStateMessage *> focusedTileStateMessages;
        std::vector<const Tile *> focusedTiles;
        TileVector2D focusPosition;
        WorldContext *worldContext = nullptr;
        Item *item = nullptr;
        std::unordered_set<AbilityType> *popupAbility = nullptr;
        uint32_t user = 0;

        void SetMouseLeftDown(bool mouseLeftDown);

        /**
         * Was it the first time that the left button was pressed?
         * 是否为首次按下左键
         * @return
         */
        [[nodiscard]] bool IsFirstMouseLeftDown() const;

        /**
        * Was it the first time that the right mouse button was pressed?
        * 是否为首次按下右键
        * @return
        */
        [[nodiscard]] bool IsFirstMouseRightDown() const;

        [[nodiscard]] bool IsMouseLeftDown() const;

        void SetMouseRightDown(bool mouseRightDown);

        [[nodiscard]] bool IsMouseRightDown() const;

    private:
        bool mouseLeftDown_ = false;
        bool mouseRightDown_ = false;
        bool firstMouseRightDown_ = false;
        bool firstMouseLeftDown_ = false;
    };
}
