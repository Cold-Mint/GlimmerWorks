//
// Created by Cold-Mint on 2026/2/26.
//

#ifndef GLIMMERWORKS_PRELOADCOLORS_H
#define GLIMMERWORKS_PRELOADCOLORS_H
#include <string>

#include "SDL3/SDL_pixels.h"


namespace glimmer {
    struct ColorResource;
    class ResourceLocator;
    /**
     * Preloaded colors
     * 预加载的颜色
     * The commonly used color values used within the game engine.
     * 用于游戏引擎内部常用的颜色值。
     */
    struct PreloadColors {
        struct ConsoleColors {
            SDL_Color titleColor;
            SDL_Color keywordColor;
            SDL_Color textColor;
            SDL_Color backgroundColor;
        };


        struct ErrorTextureColors {
            SDL_Color accentColor;
            SDL_Color baseColor;
        };

        struct GameColors {
            SDL_Color focusTileBorderColor;

            //Positive/gain attribute color
            //正向/增益属性色
            SDL_Color positiveAttributeColor;

            //Negative/curse attribute color
            //负面/诅咒属性色
            SDL_Color negativeAttributeColor;

        };

        struct DebugColor {
            SDL_Color box2dBorderColor;
            SDL_Color box2dFullColor;
            SDL_Color draggableColor;
        };

        DebugColor debugColor;

        /**
         * Default text color
         * 默认文本颜色
         */
        SDL_Color textColor;
        /**
         * Text disable color
         * 文本禁用颜色
         */
        SDL_Color textDisabledColor;
        /**
         *The color of the hyperlinked text
         *超连接颜色
         */
        SDL_Color textLinkColor;
        /**
         * The background color of the selected text in the input box / text area
         * 输入框 / 文本区域中被选中文字的背景色
         */
        SDL_Color textSelectedBgColor;
        /**
         * Background color
         * 背景颜色
         */
        SDL_Color backgroundColor;

        /**
         * Border color
         * 边框颜色
         */
        SDL_Color borderColor;

        /**
         * Border Shadow Color
         * 边框阴影颜色
         */
        SDL_Color borderShadowColor;


        /**
         *Separator line color
         * 分隔线颜色
         */
        SDL_Color separatorColor;

        /**
         * separator Hovered Color
         * 分隔线颜色
         */
        SDL_Color separatorHoveredColor;

        /**
         *Separator activation color
         * 分割线激活颜色
         */
        SDL_Color separatorActiveColor;

        /**
         * Input class control backgrounds (checkboxes, radio buttons, sliders, input boxes, dropdown boxes' backgrounds)
         * 输入类控件背景（复选框、单选框、滑块、输入框、下拉框的背景）
         */
        SDL_Color frameBgColor;
        /**
         *鼠标悬停在上述输入控件上时的背景色
         */
        SDL_Color frameBgHoveredColor;
        /**
         *鼠标点击 / 激活上述输入控件时的背景色（比如按住滑块拖动时）
         */
        SDL_Color frameBgActiveColor;
        /**
         *按钮默认背景色（普通 ImGui::Button () 的底色）
         */
        SDL_Color buttonColor;
        /**
         *鼠标悬停在按钮上时的背景色
         */
        SDL_Color buttonHoveredColor;
        /**
         *鼠标点击按钮时的背景色
         */
        SDL_Color buttonActiveColor;

        /**
         * The background color of the scroll bar (the base color of the scroll bar track)
         * 滚动条的背景色（滚动条轨道的底色）
         */
        SDL_Color scrollbarBgColor;
        /**
         * The default color of the scroll bar drag block (the draggable scroll bar slider)
         * 滚动条拖动块的默认颜色（可拖动的滚动条滑块）
         */
        SDL_Color scrollbarGrabColor;
        /**
         * The color when the mouse hovers over the slider of the scroll bar
         * 鼠标悬停在滚动条滑块上时的颜色
         */
        SDL_Color scrollbarGrabHoveredColor;
        /**
         * The color when dragging the slider of the scroll bar
         * 按住滚动条滑块拖动时的颜色
         */
        SDL_Color scrollbarGrabActiveColor;
        /**
         * The color of the cursor (caret) in the input box
         * 输入框中光标（caret）的颜色
         */
        SDL_Color inputTextCursorColor;

        /**
         * The cursor color during keyboard/handle navigation (when no mouse is used, the highlighted box of the selected control is shown using the arrow keys)
         * 键盘 / 手柄导航时的光标颜色（无鼠标时用方向键选中控件的高亮框）
         */
        SDL_Color navCursorColor;

        /**
         * 折叠面板、树节点、可选菜单项的默认背景色（比如 CollapsingHeader）
         */
        SDL_Color headerColor;
        /**
         * 鼠标悬停在上述头部控件上时的背景色
         */
        SDL_Color headerHoveredColor;
        /**
         * 点击 / 展开上述头部控件时的背景色
         */
        SDL_Color headerActiveColor;

        SDL_Color areaMarkerBorderColor;
        SDL_Color areaMarkerColor;


        ConsoleColors console;
        ErrorTextureColors error;
        GameColors game;

        void LoadAllColors(const ResourceLocator *resourceLocator);

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
        static ColorResource *LoadColor(const ResourceLocator *resourceLocator, const std::string &key,
                                        ColorResource *defaultColor);
    };
}


#endif //GLIMMERWORKS_PRELOADCOLORS_H
