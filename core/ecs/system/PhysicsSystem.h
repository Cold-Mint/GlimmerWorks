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
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class PhysicsSystem : public GameSystem {
        /**
         * Fixed time step of physical simulation (unit: seconds)
         * The physical world (such as Box2D) usually requires a fixed time step for updates.
         * Otherwise, if delta (time consumed per frame) is directly used for simulation, when the frame rate is unstable,
         * It will lead to inconsistent and unstable physical behaviors (such as speed jitter, collision anomalies, etc.).
         * The common value of FIXED_TIME_STEP is generally 1/60 second (approximately 0.016f).
         * Corresponding to a physical simulation frequency of 60Hz.
         * 物理模拟的固定时间步长（单位：秒）
         * 物理世界（例如 Box2D）通常需要使用固定时间步长来更新，
         * 否则如果用 delta（每帧耗时）直接进行模拟，当帧率不稳定时，
         * 会导致物理行为不一致、不稳定（例如速度抖动、碰撞异常等）。
         * FIXED_TIME_STEP 的常用值一般为 1/60 秒（约 0.016f），
         * 对应 60Hz 的物理模拟频率。
         */
        static constexpr float FIXED_TIME_STEP = 0.016F;
        float accumulator_ = 0.0F;

    public:
        explicit PhysicsSystem(WorldContext *worldContext);


        void Update(float delta) override;

        std::string GetName() override;
    };
}
