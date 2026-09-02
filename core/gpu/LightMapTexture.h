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

#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <vector>

namespace glimmer {
    class LightBuffer;
    struct AmbientLight;

    /**
     * LightMapTexture
     * 光照贴图纹理
     *
     * Owns the GPU texture that stores per-tile lighting (1 texel = 1 tile,
     * RGBA = light hue + intensity). The lighting fragment shader samples it
     * with bilinear filtering for smooth per-pixel lighting.
     * 持有存储逐瓦片光照（1 texel = 1 瓦片，RGBA = 光色 + 强度）的 GPU 纹理。
     * 光照片元着色器以双线性过滤采样它，得到平滑的逐像素光照。
     */
    class LightMapTexture {
        SDL_GPUDevice *device_ = nullptr;
        SDL_GPUTexture *texture_ = nullptr;
        Uint32 width_ = 0;
        Uint32 height_ = 0;
        SDL_GPUTransferBuffer *transferBuffer_ = nullptr;
        Uint32 transferBufferSize_ = 0;
        std::vector<Uint8> pixelBuffer_;
        uint64_t lastRevision_ = UINT64_MAX;
        int lastOriginX_ = 0;
        int lastOriginY_ = 0;
        Uint32 lastSizeX_ = 0;
        Uint32 lastSizeY_ = 0;
        bool lastFullBright_ = false;
        float lastAmbient_[4] = {0.0F, 0.0F, 0.0F, 0.0F};
        bool dirty_ = false;

        void EnsureTexture(Uint32 width, Uint32 height);

    public:
        ~LightMapTexture();

        LightMapTexture() = default;

        /**
         * Update
         * 从光照缓冲读取指定瓦片范围的光照（含环境光/天光）并上传到纹理。
         * @param device device GPU 设备
         * @param lightBuffer lightBuffer 光照缓冲
         * @param ambient ambient 环境光（可为 nullptr，表示无环境光）
         * @param originTileX originTileX 覆盖范围的左下角瓦片 X（最小 X）
         * @param originTileY originTileY 覆盖范围的左下角瓦片 Y（最小 Y）
         * @param sizeX sizeX 覆盖的瓦片列数
         * @param sizeY sizeY 覆盖的瓦片行数
         */
        void Update(SDL_GPUDevice *device, const LightBuffer *lightBuffer, const AmbientLight *ambient,
                    int originTileX, int originTileY, Uint32 sizeX, Uint32 sizeY,
                    bool fullBright = false);

        /**
         * Upload
         * 将暂存像素上传到纹理，必须在 command buffer 的 copy pass 内调用。
         * @param commandBuffer commandBuffer 当前帧命令缓冲
         */
        void Upload(SDL_GPUCommandBuffer *commandBuffer);

        [[nodiscard]] SDL_GPUTexture *GetTexture() const;

        [[nodiscard]] Uint32 GetWidth() const;

        [[nodiscard]] Uint32 GetHeight() const;
    };
}
