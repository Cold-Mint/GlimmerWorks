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
#include <string>

#include "core/math/Color.h"


namespace glimmer
{
    struct ColorResource;
    class ResourceLocator;
    /**
     * Preloaded colors
     * 预加载的颜色
     * The commonly used color values used within the game engine.
     * 用于游戏引擎内部常用的颜色值。
     */
    struct PreloadColors
    {
        struct ConsoleColors
        {
            Color titleColor;
            Color keywordColor;
            Color textColor;
            Color backgroundColor;
        };


        struct TextureColors
        {
            Color accentColor;
            Color baseColor;
        };

        struct GameColors
        {
            Color focusTileBorderColor;

            //Positive/gain attribute color
            //正向/增益属性色
            Color positiveAttributeColor;

            //Negative/curse attribute color
            //负面/诅咒属性色
            Color negativeAttributeColor;

            //Item slot text color(Quantity of items text color)
            //物品槽文本颜色（物品数量文本颜色）
            Color itemSlotTextColor;
        };

        struct Light2dColors
        {
            /**
            * Default emission color
            * 默认发光颜色
            */
            Color defaultEmissionColor;
            /**
             * Default transparent color
             * 默认透光颜色
             */
            Color defaultLightTransmissionColor;
        };

        struct BlueprintColors
        {
            Color validColor;
            Color invalidColor;
        };

        struct DurabilityColors
        {
            /**
            * 装备耐久度对应颜色
            * durabilityGood     : 耐久 ≥ 80%  (完好，绿色)
            * durabilityNotice   : 60% ≤ 耐久 < 80% (轻微损耗，浅橙/黄色)
            * durabilityWarning  : 10% ≤ 耐久 < 60% (中度损耗，深橙色)
            * durabilityDanger   : 耐久 < 10% (危险，红色警示)
            */
            Color durabilityGood;
            Color durabilityNotice;
            Color durabilityWarning;
            Color durabilityDanger;
        };

        struct DebugColors
        {
            Color box2dBorderColor;
            Color box2dFullColor;
            Color draggableColor;
            Color debugPanelTextColor;
            Color debugPanelTextBGColor;


            // Gradient color of the elevation map
            // 高度图渐变颜色
            Color elevationMapFrom;
            Color elevationMapTo;

            // Temperature map with gradient color changes
            // 温度图渐变颜色
            Color tempMapFrom;
            Color tempMapTo;

            // Gradation of color in the humidity map
            // 湿度图渐变颜色
            Color humidityMapFrom;
            Color humidityMapTo;

            // Erosion map gradient colorization
            // 侵蚀图渐变颜色
            Color erosionMapFrom;
            Color erosionMapTo;

            // Gradation of color in the weirdness graph
            // 怪异度图渐变颜色
            Color weirdnessMapFrom;
            Color weirdnessMapTo;
        };

        DebugColors debugColor;

        /**
         * Default text color
         * 默认文本颜色
         */
        Color textColor;
        /**
         * Text disable color
         * 文本禁用颜色
         */
        Color textDisabledColor;
        /**
         *The color of the hyperlinked text
         *超连接颜色
         */
        Color textLinkColor;
        /**
         * The background color of the selected text in the input box / text area
         * 输入框 / 文本区域中被选中文字的背景色
         */
        Color textSelectedBgColor;
        /**
         * Background color
         * 背景颜色
         */
        Color backgroundColor;

        /**
         * Border color
         * 边框颜色
         */
        Color borderColor;

        /**
         * Border Shadow Color
         * 边框阴影颜色
         */
        Color borderShadowColor;


        /**
         *Separator line color
         * 分隔线颜色
         */
        Color separatorColor;

        /**
         * separator Hovered Color
         * 分隔线颜色
         */
        Color separatorHoveredColor;

        /**
         *Separator activation color
         * 分割线激活颜色
         */
        Color separatorActiveColor;

        /**
         * Input class control backgrounds (checkboxes, radio buttons, sliders, input boxes, dropdown boxes' backgrounds)
         * 输入类控件背景（复选框、单选框、滑块、输入框、下拉框的背景）
         */
        Color frameBgColor;
        /**
         *鼠标悬停在上述输入控件上时的背景色
         */
        Color frameBgHoveredColor;
        /**
         *鼠标点击 / 激活上述输入控件时的背景色（比如按住滑块拖动时）
         */
        Color frameBgActiveColor;
        /**
         *按钮默认背景色（普通 ImGui::Button () 的底色）
         */
        Color buttonColor;
        Color buttonTextColor;
        /**
         *鼠标悬停在按钮上时的背景色
         */
        Color buttonHoveredColor;
        Color buttonHoveredTextColor;
        /**
         *鼠标点击按钮时的背景色
         */
        Color buttonPressedColor;
        Color buttonPressedTextColor;
        Color buttonDisableTextColor;

        /**
         * The background color of the scroll bar (the base color of the scroll bar track)
         * 滚动条的背景色（滚动条轨道的底色）
         */
        Color scrollbarBgColor;
        /**
         * The default color of the scroll bar drag block (the draggable scroll bar slider)
         * 滚动条拖动块的默认颜色（可拖动的滚动条滑块）
         */
        Color scrollbarGrabColor;
        /**
         * The color when the mouse hovers over the slider of the scroll bar
         * 鼠标悬停在滚动条滑块上时的颜色
         */
        Color scrollbarGrabHoveredColor;
        /**
         * The color when dragging the slider of the scroll bar
         * 按住滚动条滑块拖动时的颜色
         */
        Color scrollbarGrabActiveColor;
        /**
         * The color of the cursor (caret) in the input box
         * 输入框中光标（caret）的颜色
         */
        Color inputTextCursorColor;

        /**
         * The cursor color during keyboard/handle navigation (when no mouse is used, the highlighted box of the selected control is shown using the arrow keys)
         * 键盘 / 手柄导航时的光标颜色（无鼠标时用方向键选中控件的高亮框）
         */
        Color navCursorColor;

        /**
         * 折叠面板、树节点、可选菜单项的默认背景色（比如 CollapsingHeader）
         */
        Color headerColor;
        /**
         * 鼠标悬停在上述头部控件上时的背景色
         */
        Color headerHoveredColor;
        /**
         * 点击 / 展开上述头部控件时的背景色
         */
        Color headerActiveColor;

        Color areaMarkerBorderColor;
        Color areaMarkerColor;


        ConsoleColors console;
        TextureColors error;
        TextureColors accessDenied;
        GameColors game;
        Light2dColors light;
        BlueprintColors blueprint;
        DurabilityColors durability;

        void LoadAllColors(const ResourceLocator* resourceLocator);

    private:
        /**
         * Load color
         * 加载颜色
         * Only supports loading the colors of Core data.
         * 仅支持加载Core数据的颜色。
         * @param resourceLocator 资源定位器
         * @param key 路径
         * @param defaultColor 默认颜色
         * @return
         */
        static Color LoadColor(const ResourceLocator* resourceLocator, const std::string& key,
                               const Color& defaultColor);
    };
}
