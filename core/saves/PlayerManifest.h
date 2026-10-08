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
#include "core/mod/ResourceRef.h"
#include "src/core/player.pb.h"

namespace glimmer
{
    /**
     * PlayerManifest
     * 玩家的清单文件
     */
    struct PlayerManifest
    {
    private:
        std::vector<PlayerDimensionMessage> visitedDimensions_;
        uint32_t currentDimensionIndex_ = 0;

    public:
        EntityItemMessage entityItemMessage;
        long lastPlayedTime = 0;
        PlayerPermissionLevelMessage permissionLevel = PLAYER_PERMISSION_LEVEL_NORMAL;

        /**
         * Check if the player has visited a certain dimension.
         * 获取玩家是否访问过某个维度。
         * @param dimensionsResourceRef
         * @return 如果为-1表示未访问过。
         */
        uint32_t Visited(const ResourceRef& dimensionsResourceRef) const;


        /**
         * SwitchDimension
         * 切换到某个维度
         * @param dimensionsResourceRef
         */
        void SwitchDimension(const ResourceRef& dimensionsResourceRef);

        /**
         * CurrentDimension
         * 获取当前的维度信息
         * @return If the retrieval fails, return nullptr. 如果获取不到返回nullptr
         */
        const PlayerDimensionMessage* GetCurrentDimension() const;


        void FromMessage(const PlayerMessage& playerMessage);

        void ToMessage(PlayerMessage& playerMessage) const;
    };
}
