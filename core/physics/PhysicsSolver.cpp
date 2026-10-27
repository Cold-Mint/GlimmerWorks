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
#include "PhysicsSolver.h"

#include "SolveResult.h"
#include "core/ecs/component/ColliderComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/log/LogCat.h"
#include "core/math/Vector2D.h"
#include "core/world/ChunkManager.h"

void glimmer::PhysicsSolver::SetHorizontalPhysicsSolver(
    std::unique_ptr<IHorizontalPhysicsSolver> horizontalPhysicsSolver) {
    horizontalPhysicsSolver_ = std::move(horizontalPhysicsSolver);
    LogCat::d(LogLabel::PHYSICS, "physics_solver_set_horizontal",
              "Set horizontal physics solver: {}", horizontalPhysicsSolver_ != nullptr);
}

void glimmer::PhysicsSolver::SetVerticalPhysicsSolver(std::unique_ptr<IVerticalPhysicsSolver> verticalPhysicsSolver) {
    verticalPhysicsSolver_ = std::move(verticalPhysicsSolver);
    LogCat::d(LogLabel::PHYSICS, "physics_solver_set_vertical",
              "Set vertical physics solver: {}", verticalPhysicsSolver_ != nullptr);
}

void glimmer::PhysicsSolver::Solver(const ChunkManager *chunkManager, ColliderComponent *colliderComponent,
                                    Transform2DComponent *transform2dComponent) {
    if (horizontalPhysicsSolver_ == nullptr || verticalPhysicsSolver_ == nullptr) {
        LogCat::w(LogLabel::PHYSICS, std::source_location::current(), "physics_solver_missing",
                  "Physics solver is not fully configured (horizontal={}, vertical={}), skip solving",
                  horizontalPhysicsSolver_ != nullptr, verticalPhysicsSolver_ != nullptr);
        return;
    }
    const WorldAABB box = colliderComponent->GetAABB(transform2dComponent->GetPosition());
    const WorldVector2D &velocity = colliderComponent->GetVelocity();
    const float gravityScale = colliderComponent->GetGravityScale();

    LogCat::d(LogLabel::PHYSICS, "physics_solver_start",
              "Start physics solve: center=({}, {}), half=({}, {}), velocity=({}, {}), gravityScale={}",
              box.center.x, box.center.y, box.halfWidth, box.halfHeight, velocity.x, velocity.y, gravityScale);

    //Solve the vertical direction (gravity is applied inside the vertical solver).
    //解算竖直方向（重力在竖直求解器内应用）。
    const SolveResult verticalSolveResult = verticalPhysicsSolver_->SolveYDirection(
        chunkManager, box, velocity.y, gravityScale);
    //Solve the horizontal direction.
    //解算水平方向。
    const SolveResult horizontalSolveResult = horizontalPhysicsSolver_->SolveXDirection(
        chunkManager, box, velocity.x);

    //Write back the corrected velocity and the grounded state.
    //写回修正后的速度与落地状态。
    colliderComponent->SetVelocity(WorldVector2D{horizontalSolveResult.velocity, verticalSolveResult.velocity});
    colliderComponent->SetGrounded(verticalSolveResult.grounded);

    //Move the position by the measured distance.
    //位置按测量出的距离移动。
    transform2dComponent->SetPosition(box.center
                                      + WorldVector2D{horizontalSolveResult.distance, verticalSolveResult.distance});

    LogCat::d(LogLabel::PHYSICS, "physics_solver_result",
              "Physics solve done: newVelocity=({}, {}), distance=({}, {}), grounded={}, vBlocked={}, hBlocked={}",
              horizontalSolveResult.velocity, verticalSolveResult.velocity,
              horizontalSolveResult.distance, verticalSolveResult.distance,
              verticalSolveResult.grounded, verticalSolveResult.blocked, horizontalSolveResult.blocked);
}
