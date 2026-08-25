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

namespace glimmer {
    /**
     * GpuTexture
     * GPU 纹理
     *
     * Lightweight wrapper around SDL_GPUTexture that also stores the pixel size
     * (replacing SDL_Texture in the old renderer based code, so `w`/`h` remain
     * directly accessible like before).
     * SDL_GPUTexture 的轻量封装，同时保存像素尺寸（替代旧渲染器代码中的 SDL_Texture，
     * 因此 w/h 仍可像之前一样直接访问）。
     *
     * The texture is created by GpuContext::CreateTextureFromSurface and released
     * by calling Destroy() (or implicitly by the destructor).
     * 纹理由 GpuContext::CreateTextureFromSurface 创建，通过调用 Destroy() 释放
     * （或由析构函数隐式释放）。
     */
    class GpuTexture {
        SDL_GPUDevice *device_ = nullptr;
        SDL_GPUTexture *texture_ = nullptr;

    public:
        /**
         * Width of the texture in pixels.
         * 纹理宽度（像素）。
         */
        int w = 0;
        /**
         * Height of the texture in pixels.
         * 纹理高度（像素）。
         */
        int h = 0;

        GpuTexture() = default;

        GpuTexture(SDL_GPUDevice *device, SDL_GPUTexture *texture, int width, int height);

        GpuTexture(const GpuTexture &) = delete;

        GpuTexture &operator=(const GpuTexture &) = delete;

        ~GpuTexture();

        /**
         * Release the underlying GPU texture. Safe to call multiple times.
         * 释放底层 GPU 纹理。可安全地多次调用。
         */
        void Destroy();

        /**
         * @return The raw SDL_GPUTexture handle, nullptr if destroyed.
         * 原始 SDL_GPUTexture 句柄，已销毁时返回 nullptr。
         */
        [[nodiscard]] SDL_GPUTexture *GetGpuTexture() const;

        /**
         * @return True if the texture has not been destroyed.
         * 纹理未被销毁时返回 true。
         */
        [[nodiscard]] bool IsValid() const;
    };
}
