//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_CAMERACOMPONENT_H
#define GLIMMERWORKS_CAMERACOMPONENT_H
#include "../GameComponent.h"
#include <SDL3/SDL_rect.h>

#include "../../math/Vector2D.h"

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
        Vector2D size_ = {800.0F, 600.0F};

        /**
         * zoom camera zoom ratio
         * zoom 相机缩放比例
         */
        float zoom_ = 1.0F;

    public:
        /**
         * Get the camera viewport rectangle (in pixel coordinates)
         * 获取相机的视口矩形（像素坐标）
         * @return SDL_FRect Viewport rectangle 视口矩形
         */
        [[nodiscard]] SDL_FRect GetViewportRect(WorldVector2D cameraPosition) const;


        /**
         * Get the camera viewport position (in pixel coordinates)
         * 获取相机的视口位置（像素坐标）
         * @param cameraPosition The camera position (in pixel coordinates) 相机位置（像素坐标）
         * @param worldPosition The world position (in pixel coordinates) 世界位置（像素坐标）
         * @return The camera viewport position (in pixel coordinates) 相机视口位置（像素坐标）
         */
        [[nodiscard]] CameraVector2D GetViewPortPosition(WorldVector2D cameraPosition,
                                                         WorldVector2D worldPosition) const;


        /**
         * Is point in viewport
         * 判断世界坐标是否在相机视口内
         * @param worldPos worldPos 世界坐标
         * @return 是否在相机视口内
         */
        [[nodiscard]] bool IsPointInViewport(WorldVector2D cameraPosition, WorldVector2D worldPos) const;


        /**
         * SetSize
         * 设置尺寸
         * @param size Size 尺寸
         */
        void SetSize(Vector2D size);

        /**
         * Get Size
         * 获取相机尺寸
         * @return
         */
        [[nodiscard]] Vector2D GetSize() const;


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
    };
}

#endif //GLIMMERWORKS_CAMERACOMPONENT_H
