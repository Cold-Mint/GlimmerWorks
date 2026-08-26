/*
 * Copyright (C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
 * 版权(C) 2025-2026  Cold-Mint <cold_mint@qq.com>
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
#include <vector>

#include <SDL3/SDL_gpu.h>

#include "DrawRun.h"
#include "RenderCommand.h"
#include "SpriteVertex.h"

namespace glimmer {
    class GpuContext;
    class GpuTexture;

    /**
     * GpuSpriteBatcher
     * GPU 精灵合批器
     *
     * Turns a sorted list of RenderCommand objects into GPU vertex data,
     * groups consecutive commands that share the same texture into DrawRuns,
     * uploads the vertices to a dynamic GPU buffer and draws the runs.
     * 将排序后的 RenderCommand 列表转换为 GPU 顶点数据，把共享同一纹理的
     * 连续命令合并为 DrawRun，上传顶点到动态 GPU 缓冲并绘制这些段。
     */
    class GpuSpriteBatcher {
        static constexpr Uint32 INITIAL_VERTEX_CAPACITY = 65536;

        SDL_GPUBuffer *vertexBuffer_ = nullptr;
        Uint32 vertexBufferCapacity_ = 0;
        std::unique_ptr<GpuTexture> whiteTexture_;

        void GrowBuffer(SDL_GPUDevice *device, size_t requiredVertexCount);

        static void ExpandRange(const RenderCommand *commands, size_t count, const GpuTexture *whiteTexture,
                                std::vector<SpriteVertex> &vertices, std::vector<DrawRun> &runs);

    public:
        GpuSpriteBatcher() = default;

        /**
         * Create the dynamic vertex buffer and the 1x1 white texture.
         * 创建动态顶点缓冲和 1x1 白色纹理。
         */
        bool Init(GpuContext *context);

        /**
         * Release the vertex buffer and white texture.
         * 释放顶点缓冲和白色纹理。
         */
        void Shutdown(SDL_GPUDevice *device);

        /**
         * Expand commands into vertices and texture-grouped draw runs.
         * The output vectors are appended to so two ranges can share one upload.
         * 将命令展开为顶点和按纹理分组的绘制段。输出向量采用追加方式，
         * 以便两段范围可以共享一次上传。
         */
        struct Batch {
            std::vector<SpriteVertex> vertices;
            std::vector<DrawRun> worldRuns;
            std::vector<DrawRun> overlayRuns;
        };

        Batch Build(const std::vector<RenderCommand> &commands, size_t splitIndex);

        /**
         * Upload the vertices to the GPU vertex buffer, growing it if needed.
         * 将顶点上传到 GPU 顶点缓冲，需要时扩充缓冲。
         * @return true if the upload succeeded and drawing can proceed.
         * 上传成功且可以继续绘制时返回 true。
         */
        bool Upload(SDL_GPUDevice *device, SDL_GPUCommandBuffer *commandBuffer,
                    const std::vector<SpriteVertex> &vertices);

        /**
         * Draw a list of runs using the currently bound pipeline.
         * 使用当前绑定的管线绘制一段列表。
         */
        static void DrawRuns(SDL_GPURenderPass *renderPass, const std::vector<DrawRun> &runs,
                             SDL_GPUSampler *sampler);

        [[nodiscard]] SDL_GPUBuffer *GetVertexBuffer() const { return vertexBuffer_; }

        [[nodiscard]] GpuTexture *GetWhiteTexture() const { return whiteTexture_.get(); }
    };
}
