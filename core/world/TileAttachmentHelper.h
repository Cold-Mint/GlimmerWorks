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

#include "core/math/Vector2DI.h"
#include "core/math/TileVector2D.h"
#include "core/world/TileAttachmentType.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer {
    class Tile;
    class WorldContext;

    /**
     * TileAttachmentHelper
     * 瓦片附着点工具类：负责根据向量A解析附着方向、验证附着支撑、计算附着偏移。
     */
    class TileAttachmentHelper {
        static const Tile *GetTileAt(const WorldContext *worldContext, TileLayerType layerType,
                                     const TileVector2D &cell);

    public:
        /**
         * DeterminePriorityDirection
         * 根据玩家到放置点的向量A，计算优先检测的附着方向。
         * 卡迪纳尔方向与向量A一致；零向量优先背景(下层)；对角默认上方。
         * @param vectorA 向量A（放置点 - 玩家，瓦片坐标）
         * @return 优先附着方向（掩码位）
         */
        static TileAttachmentType DeterminePriorityDirection(const TileVector2D &vectorA);

        /**
         * BuildSearchOrder
         * 构建附着方向检测顺序：优先方向 -> 顺时针 -> 背景(下层)。
         * 零向量（背景优先）时顺序为 背景 -> 上 -> 右 -> 下 -> 左。
         * @param priority 优先方向
         * @return 检测顺序
         */
        static std::vector<TileAttachmentType> BuildSearchOrder(TileAttachmentType priority);

        /**
         * HasAnyTag
         * 判断瓦片是否拥有指定标签集合中的任一标签（按缓存ID匹配）。
         */
        static bool HasAnyTag(const Tile *tile, const std::vector<uint64_t> &tagIds);

        /**
         * IsSupportValid
         * 判断瓦片在指定附着方向上的整条支撑边是否全部有效（每个支撑格都存在且拥有任一标签）。
         * @param worldContext
         * @param layerType 瓦片所在图层
         * @param tileTopLeft 瓦片左上角坐标
         * @param width 瓦片宽度
         * @param height 瓦片高度
         * @param direction 附着方向
         * @param tagIds 目标瓦片必须拥有的标签ID列表（任一匹配即可）
         */
        static bool IsSupportValid(const WorldContext *worldContext, TileLayerType layerType,
                                   const TileVector2D &tileTopLeft, uint8_t width, uint8_t height,
                                   TileAttachmentType direction, const std::vector<uint64_t> &tagIds);

        /**
         * FindAttachment
         * 按优先级与顺时针顺序查找第一个有效的附着方向。
         * @param outDirection 输出解析出的附着方向
         * @param outOffset 输出附着点单位偏移（相对瓦片原点，指向支撑瓦片）
         * @return 是否找到有效附着点
         */
        static bool FindAttachment(const WorldContext *worldContext, TileLayerType layerType,
                                   const TileVector2D &tileTopLeft, uint8_t width, uint8_t height,
                                   uint8_t attachmentMask, const std::vector<uint64_t> &tagIds,
                                   const TileVector2D &vectorA,
                                   TileAttachmentDirection &outDirection, Vector2DI &outOffset);

        /**
         * ValidateAttachment
         * 校验已附着瓦片的支撑是否仍然有效（用于加载后校验与破坏级联）。
         */
        static bool ValidateAttachment(const WorldContext *worldContext, TileLayerType layerType,
                                       const TileVector2D &tileTopLeft, uint8_t width, uint8_t height,
                                       const std::vector<uint64_t> &tagIds,
                                       TileAttachmentDirection direction);

        /**
         * GetAttachmentOffset
         * 获取附着方向的单位偏移向量（相对瓦片原点，指向支撑瓦片）。
         */
        static Vector2DI GetAttachmentOffset(TileAttachmentType direction);

        static TileAttachmentDirection ToDirection(TileAttachmentType type);

        static TileAttachmentType ToMaskType(TileAttachmentDirection direction);
    };
}
