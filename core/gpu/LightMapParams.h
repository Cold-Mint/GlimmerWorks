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

namespace glimmer {
    /**
     * LightMapParams
     * 光照贴图参数
     *
     * Uniform block consumed by the lighting fragment shader
     * (shaders/@core/lighting.frag, set = 3, binding = 0). The layout must
     * match the GLSL std140 declaration exactly (4 vec2 slots followed by
     * 4 floats, 48 bytes total).
     * 光照片元着色器（shaders/@core/lighting.frag，set = 3，binding = 0）
     * 消费的 uniform 块。布局必须与 GLSL std140 声明完全一致
     * （4 个 vec2 槽位后跟 4 个 float，共 48 字节）。
     */
    struct LightMapParams {
        /**
         * Tile coordinate stored in light map texel (0,0) (left/top edge of
         * the covered area, in tile units).
         * 光照贴图 texel(0,0) 存储的瓦片坐标（覆盖区域的左/上边缘，瓦片单位）。
         */
        float lightMapOriginX = 0.0F;
        float lightMapOriginY = 0.0F;
        /**
         * Light map texture size in texels (1 texel = 1 tile).
         * 光照贴图纹理尺寸（texel，1 texel = 1 瓦片）。
         */
        float lightMapSizeX = 1.0F;
        float lightMapSizeY = 1.0F;
        /**
         * Fractional tile coordinate at the screen's top-left corner.
         * 屏幕左上角的小数瓦片坐标。
         */
        float cameraTopLeftTileX = 0.0F;
        float cameraTopLeftTileY = 0.0F;
        /**
         * Viewport size in tiles.
         * 视口尺寸（瓦片单位）。
         */
        float viewportTilesX = 1.0F;
        float viewportTilesY = 1.0F;
        /**
         * Light intensity (0..1, light alpha / 255) that maps to full
         * brightness; lower values brighten dim areas.
         * 映射为全亮的光照强度（0..1，光照 alpha / 255）；值越小暗区越亮。
         */
        float fullBright = 0.05F;
        /**
         * Minimum visibility floor applied to pixels with no light at all
         * (keeps entities faintly visible in darkness).
         * 完全无光像素应用的最低可见度下限（让实体在黑暗中保持隐约可见）。
         */
        float minVisibility = 0.05F;
        /**
         * Light hue tint strength, 0 = neutral gray lighting,
         * 1 = full hue filtering.
         * 光色染色强度，0 = 中性灰光照，1 = 完整色相过滤。
         */
        float tintStrength = 0.6F;
        float padding = 0.0F;
    };
}
