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

namespace glimmer {
    struct RenderFrameContext;

    /**
     * Pass
     * 渲染通道
     *
     * One logical step of the frame render pipeline (scene geometry, lighting
     * composite, UI, ...). Each pass records its GPU commands against the
     * shared RenderFrameContext. A pass may also do CPU-side preparation
     * (Prepare) before recording.
     * 帧渲染流水线中的一个逻辑步骤（场景几何、光照合成、UI 等）。每个 pass
     * 基于共享的 RenderFrameContext 记录其 GPU 命令。pass 也可以在记录前
     * 执行 CPU 端准备（Prepare）。
     */
    class IPass {
    public:
        virtual ~IPass() = default;

        /**
         * CPU-side preparation executed before Record. The default does
         * nothing; passes that need per-frame CPU work (e.g. rebuilding the
         * light map) override it.
         * Record 之前执行的 CPU 端准备。默认为空；需要每帧 CPU 工作
         * （如重建光照贴图）的 pass 重写它。
         */
        virtual void Prepare(RenderFrameContext &ctx);

        /**
         * Record GPU commands for this pass into the frame command buffer.
         * 将本 pass 的 GPU 命令记录到帧命令缓冲。
         */
        virtual void Record(RenderFrameContext &ctx) = 0;
    };
}
