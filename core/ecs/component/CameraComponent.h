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
#include "core/ecs/GameComponent.h"
#include <SDL3/SDL_rect.h>

#include "core/math/ScreenVector2D.h"
#include "core/math/WorldVector2D.h"

namespace glimmer
{
    /**
     * Camera component, used to control the game view
     * 相机组件，用于控制游戏视图
     */
    class CameraComponent : public GameComponent
    {
        /**
         * Size: Camera size (pixel coordinates)
         * Size 相机尺寸（像素坐标）
         */
        ScreenVector2D size_ = {800.0F, 600.0F};

        /**
         * zoom camera zoom ratio
         * zoom 相机缩放比例
         */
        float zoom_ = 2.0F;

    public:
        /**
         * Is point in viewport
         * 判断世界坐标是否在相机视口内
         * @param cameraPosition cameraPosition 相机坐标
         * @param worldPos worldPos 世界坐标
         * @return 是否在相机视口内
         */
        [[nodiscard]] bool IsPointInViewport(const WorldVector2D& cameraPosition, const WorldVector2D& worldPos) const;

        /**
         * Is rect in viewport
         * 判断矩形是否在相机视口内
         * @param cameraPosition
         * @param rect
         * @return
         */
        [[nodiscard]] bool IsRectInViewport(const WorldVector2D& cameraPosition, const SDL_FRect* rect) const;


        /**
         * SetSize
         * 设置尺寸
         * @param size Size 尺寸
         */
        void SetSize(const ScreenVector2D& size);

        /**
         * Get Size
         * 获取相机尺寸
         * @return
         */
        [[nodiscard]] ScreenVector2D GetSize() const;


        /**
         * Get the camera zoom factor
         * 获取相机缩放比例
         * @return The camera zoom factor 相机缩放比例
         */
        [[nodiscard]] float GetZoom() const;

        /**
         * Set the camera zoom factor
         * 设置相机缩放比例
         * @param zoom The camera zoom factor 相机缩放比例
         */
        void SetZoom(float zoom);

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
