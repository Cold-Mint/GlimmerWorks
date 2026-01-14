//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_DEBUGDRAWCOMPONENT_H
#define GLIMMERWORKS_DEBUGDRAWCOMPONENT_H
#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "SDL3/SDL_pixels.h"

namespace glimmer {
    class DebugDrawComponent : public GameComponent {
        Vector2D size_;

        SDL_Color color_ = {255, 255, 255, 255};

    public:
        /**
         * GetColor
         * 获取渲染的颜色
         * @return
         */
        [[nodiscard]] SDL_Color GetColor() const;


        /**
         * Set Color
         * 设置颜色
         * @param color color 颜色
         */
        void SetColor(const SDL_Color &color);

        /**
         * Get Size
         * 获取尺寸
         * @return Size 尺寸
         */
        [[nodiscard]] Vector2D GetSize() const;

        /**
         *SetSize
         * 设置尺寸
         * @param size Size 尺寸
         */
        void SetSize(const Vector2D &size);

        [[nodiscard]] u_int32_t GetId() override;
    };
}


#endif //GLIMMERWORKS_DEBUGDRAWCOMPONENT_H
