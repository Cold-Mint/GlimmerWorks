/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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

namespace glimmer {
    /**
     * RenderLayer
     * 渲染层
     *
     * The layered render queue draws commands layer by layer, in ascending
     * order of this enum (painter's algorithm: lower layers are drawn first
     * and appear behind higher ones). Inside one layer, commands are ordered
     * by their depth value; commands with equal depth keep their submission
     * order (stable sort).
     * 分层渲染队列按此枚举的升序逐层绘制命令（画家算法：数值小的层先绘制，
     * 位于数值大的层之下）。同一层内命令按 depth 值排序，depth 相同的命令
     * 保持提交顺序（稳定排序）。
     *
     * All layers are rendered into the offscreen game layer; RmlUi always
     * composites above them (see GpuRenderer).
     * 所有层都渲染进离屏 game 层；RmlUi 始终合成在它们之上（见 GpuRenderer）。
     */
    enum class RenderLayer : std::uint8_t {
        /**
         * Parallax backgrounds behind everything else.
         * 位于一切之后的视差背景。
         */
        Background = 0,
        /**
         * Tile layers (ground / background walls).
         * 瓦片层（地面/背景墙）。
         */
        Tile,
        /**
         * Dropped items lying on the ground.
         * 落在地面上的掉落物。
         */
        GroundItem,
        /**
         * Overlays attached to tiles or the ground: mining cracks, area
         * markers.
         * 附着在瓦片或地面上的叠加物：挖掘裂纹、区域标记。
         */
        TileOverlay,
        /**
         * Entity sprites (player, creatures).
         * 实体精灵（玩家、生物）。
         */
        Entity,
        /**
         * World-space effects: floating texts, dragged items, blueprint
         * placement previews.
         * 世界空间特效：飘字、拖拽中的物品、蓝图放置预览。
         */
        Effect,
        /**
         * 2D lighting overlay.
         * 2D 光照叠加层。
         */
        Lighting,
        /**
         * Debug geometry: physics wireframes, debug rectangles, debug maps.
         * 调试几何图形：物理线框、调试矩形、调试地图。
         */
        Debug,
        /**
         * Screen-space overlays: debug panel, virtual touch controls, UI
         * messages.
         * 屏幕空间覆盖物：调试面板、虚拟触控按键、UI 消息。
         */
        Overlay,
    };
}
