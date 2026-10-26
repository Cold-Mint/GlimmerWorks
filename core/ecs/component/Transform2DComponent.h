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
#include "core/math/WorldVector2D.h"
#include "core/utils/VersionAble.h"

namespace glimmer {
    /**
     * The world coordinate component is used to record the absolute position of objects in the 2D game world
     * 世界坐标组件，用于记录物体在2D游戏世界中的绝对位置
     */
    class Transform2DComponent : public GameComponent, public VersionAble {
        //Latest position written by the tick thread.
        //tick 线程写入的最新位置。
        WorldVector2D pendingPosition_;
        //Atomic snapshot of pendingPosition_, so other threads can read the
        //latest position without racing the tick thread's writes.
        //pendingPosition_ 的原子快照，供其它线程无竞争地读取最新位置。
        std::atomic<uint64_t> pendingSnapshot_{0};
        //Interpolation endpoints and the resolved render position, all owned
        //by the render thread and updated once per frame in Commit.
        //插值端点与解析出的渲染位置，均由渲染线程所有，每帧 Commit 更新一次。
        WorldVector2D previousPosition_;
        WorldVector2D currentPosition_;
        WorldVector2D renderPosition_;
        //Whether the position has been committed at least once; the first
        //commit aligns instead of interpolating to avoid a teleport from (0,0).
        //位置是否已提交过至少一次；首次提交直接对齐而非插值，避免从 (0,0) 瞬移。
        bool firstCommit_ = true;

    public:
        Transform2DComponent() = default;

        explicit Transform2DComponent(const WorldVector2D &position);

        /**
         * SetPosition 设置世界坐标
         * @param newPosition newPosition 新的坐标
         */
        void SetPosition(const WorldVector2D &newPosition);

        /**
         * Submit the pending position for this frame and resolve the render
         * position by interpolating between the two latest positions.
         * 提交本帧的待定位置，并在最近两个位置之间插值得到渲染位置。
         * @param alpha interpolation factor in [0, 1] 插值因子，取值 [0, 1]
         */
        void Commit(float alpha);

        /**
         * GetPosition 获取最新世界坐标
         * @return 世界坐标
         */
        [[nodiscard]] const WorldVector2D &GetPosition() const;

        /**
         * GetRenderPosition 获取本帧插值后的渲染位置
         * @return 渲染位置
         */
        [[nodiscard]] const WorldVector2D &GetRenderPosition() const;

        [[nodiscard]] std::optional<std::string> Serialize() override;

        void Deserialize(WorldContext *worldContext, const std::string &data) override;

        [[nodiscard]] static GameComponentTypeMessage GetComponentTypeStatic();

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
