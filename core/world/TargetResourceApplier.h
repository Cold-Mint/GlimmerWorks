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
#include "BreakSource.h"
#include "core/math/TileVector2D.h"
#include "core/mod/ResourceRef.h"
#include "generator/TileLayerType.h"
#include "src/core/place_source_message.pb.h"

namespace glimmer {
    class WorldContext;

    /**
     * TargetResourceApplier
     * 目标资源应用器
     *
     * Applies a target ResourceRef at a tile position, dispatching on the target's
     * resource type (tile / item / loot table / structure). Shared by the crop
     * growth system and the interaction system so that the "resolve a target and
     * apply it" logic is maintained in a single place.
     * The target is applied at originTilePosition plus the origin tile's
     * spawnOriginOffset for every resource type (tile included).
     * 在瓦片位置应用一个目标资源引用，并根据目标资源类型（瓦片/物品/战利品表/结构）分发处理。
     * 由作物生长系统与交互系统共用，从而让"解析并应用目标"的逻辑只维护在一处。
     * 对所有资源类型（包括瓦片）而言，目标都应用在"原点瓦片位置 + 原点瓦片的 spawnOriginOffset"处。
     */
    struct TargetResourceApplier final {
        /**
         * Apply
         * 应用目标资源
         *
         * @param worldContext worldContext 世界上下文
         * @param originTilePosition originTilePosition 目标应用的原点瓦片位置（绝对瓦片坐标）
         * @param layerType layerType 图层类型
         * @param targetRef targetRef 目标资源引用
         * @param breakSource breakSource 覆盖/放置时的破坏来源
         * @param placeSource placeSource 放置来源
         * @return Whether the target was applied successfully 是否成功应用目标
         */
        static bool Apply(WorldContext *worldContext,
                          const TileVector2D &originTilePosition,
                          TileLayerType layerType,
                          const ResourceRef &targetRef,
                          BreakSource breakSource,
                          PlaceSourceMessage placeSource);
    };
} // namespace glimmer
