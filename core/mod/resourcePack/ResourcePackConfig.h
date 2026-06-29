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
//@genCode
namespace glimmer
{
    //@genNextLine(NineSliceConfig|九切片通用配置，存储九宫格四边裁切尺寸与平铺开关)
    struct NineSliceConfig
    {
        //@genNextLine(leftBorderPx|九切片左侧边框像素宽度)
        float leftBorderPx = 1.0F;

        //@genNextLine(rightBorderPx|九切片右侧边框像素宽度)
        float rightBorderPx = 1.0F;

        //@genNextLine(topBorderPx|九切片顶部边框像素高度)
        float topBorderPx = 1.0F;

        //@genNextLine(bottomBorderPx|九切片底部边框像素高度)
        float bottomBorderPx = 1.0F;

        //@genNextLine(enableTiled|九切片中间区域是否启用瓦片平铺模式)
        bool enableTiled = false;

        //@genNextLine(scale|缩放)
        float scale = 0.0F;

        //@genNextLine(tileScale|平铺纹理缩放倍率)
        float tileScale = 1.0F;
    };


    //@genNextLine(ResourcePackConfig|资源包配置)
    struct ResourcePackConfig
    {
        //@genNextLine(button|按钮九切片配置)
        NineSliceConfig buttonNineSlice = {};

        //@genNextLine(itemSlot|物品槽九切片配置)
        NineSliceConfig itemSlotNineSlice = {};

        //@genNextLine(itemSlotQuantity|物品槽数量框九切片配置)
        NineSliceConfig itemSlotQuantityNineSlice = {};

        //@genNextLine(craftPreviewSlot|合成预览框九切片配置)
        NineSliceConfig craftPreviewSlotNineSlice = {};

        //@genNextLine(itemToolTip|物品工具提示配置)
        NineSliceConfig itemToolTipNineSlice = {};
    };
}
