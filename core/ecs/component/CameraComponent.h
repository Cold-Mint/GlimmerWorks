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
     * 相机组件，用于控制游戏视图
     */
    class CameraComponent : public GameComponent
    {
        /**
         * Size 相机尺寸（像素坐标）
         */
        Vector2D size_ = {800.0f, 600.0f};

        /**
        * speed 相机速度（单位：像素/秒）
        */
        float speed_ = 100.0f;

        /**
         * zoom 相机缩放比例
         */
        float zoom_ = 1.0f;

    public:
        /**
         * Get the camera viewport rectangle (in pixel coordinates)
         * 获取相机的视口矩形（像素坐标）
         * @return SDL_FRect Viewport rectangle 视口矩形
         */
        [[nodiscard]] SDL_FRect GetViewportRect(Vector2D cameraPosition) const;


        /**
         * Get the camera viewport position (in pixel coordinates)
         * 获取相机的视口位置（像素坐标）
         * @param cameraPosition The camera position (in pixel coordinates) 相机位置（像素坐标）
         * @param worldPosition The world position (in pixel coordinates) 世界位置（像素坐标）
         * @return The camera viewport position (in pixel coordinates) 相机视口位置（像素坐标）
         */
        [[nodiscard]] Vector2D GetViewPortPosition(Vector2D cameraPosition, Vector2D worldPosition) const;



        /**
         * Set the camera speed
         * 设置相机速度
         * @param speed The camera speed (units: pixels/second) 相机速度（单位：像素/秒）
         */
        void SetSpeed(float speed);


        /**
         * Get the camera speed
         * 获取相机速度
         * @return The camera speed (units: pixels/second) 相机速度（单位：像素/秒）
         */
        [[nodiscard]] float GetSpeed() const;

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
