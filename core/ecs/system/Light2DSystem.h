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
#include <cstdint>
#include <vector>

#include "core/ecs/GameSystem.h"
#include "core/world/Tile.h"


namespace glimmer {
    class CameraComponent;
    class Transform2DComponent;

    /**
     * Light2DSystem
     * 2D 光照系统
     *
     * Builds the per-tile light map (1 texel = 1 tile, RGBA = light hue +
     * intensity) for the visible viewport and hands it to the GpuRenderer
     * (see GpuRenderer::SetLightMap). The actual per-pixel lighting is
     * applied on the GPU by the lighting fragment shader
     * (shaders/@core/lighting.frag) with smooth bilinear interpolation.
     * 为可见视口构建逐瓦片光照贴图（1 texel = 1 瓦片，RGBA = 光色 + 强度），
     * 并交给 GpuRenderer（见 GpuRenderer::SetLightMap）。真正的逐像素光照由
     * GPU 上的光照片元着色器（shaders/@core/lighting.frag）以平滑的
     * 双线性插值完成。
     *
     * The light map is only rebuilt when the covered tile area or the light
     * buffer revision changes; otherwise the previous GPU texture is reused
     * and only the shader parameters are refreshed.
     * 光照贴图仅在覆盖的瓦片区域或光照缓冲修订号变化时重建；
     * 否则复用先前的 GPU 纹理，仅刷新着色器参数。
     */
    class Light2DSystem : public GameSystem {
        CameraComponent *cameraComponent_ = nullptr;
        Transform2DComponent *cameraTransform2DComponent_ = nullptr;

        /**
         * CPU-side staging pixels and the cache keys of the last build.
         * CPU 侧暂存像素与上次构建的缓存键。
         */
        std::vector<uint8_t> pixelBuffer_;
        int lastWidth_ = 0;
        int lastHeight_ = 0;
        int lastOriginX_ = 0;
        int lastOriginY_ = 0;
        uint64_t lastRevision_ = 0;
        bool hasCache_ = false;

    public:
        explicit Light2DSystem(WorldContext *worldContext);

        void OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) override;

        uint8_t GetExecutionOrder() override;

        void Render(RenderQueue *queue) override;

        [[nodiscard]] GameSystemType GetGameSystemType() const override;
    };
}
