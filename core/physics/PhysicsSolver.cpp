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
#include "core/math/CoordinateTransformer.h"
#include "core/math/Vector2D.h"
#include "core/world/ChunkManager.h"

void glimmer::PhysicsSolver::SetHorizontalPhysicsSolver(
    std::unique_ptr<IHorizontalPhysicsSolver> horizontalPhysicsSolver) {
    horizontalPhysicsSolver_ = std::move(horizontalPhysicsSolver);
}

void glimmer::PhysicsSolver::SetVerticalPhysicsSolver(std::unique_ptr<IVerticalPhysicsSolver> verticalPhysicsSolver) {
    verticalPhysicsSolver_ = std::move(verticalPhysicsSolver);
}

void glimmer::PhysicsSolver::Solver(const ChunkManager *chunkManager, const ColliderComponent *colliderComponent,
                                    Transform2DComponent *transform2dComponent) {
    if (horizontalPhysicsSolver_ == nullptr || verticalPhysicsSolver_ == nullptr) {
        return;
    }
    const WorldAABB box = colliderComponent->GetAABB(transform2dComponent->GetPosition());
    WorldVector2D velocityCopy = colliderComponent->GetVelocity();
    const float gravityScale = colliderComponent->GetGravityScale();

    SolveResult verticalSolveResult;
    if (velocityCopy.y != 0.0F) {
        //Only when the vertical velocity is not zero, will the vertical direction be calculated.
        //只有垂直速度不为0时才解算垂直方向。
        // verticalSolveResult = verticalPhysicsSolver_->SolveYDirection(box, velocityCopy.y, gravityScale);
    }
    SolveResult horizontalSolveResult;
    if (velocityCopy.x != 0.0F) {
        horizontalSolveResult = horizontalPhysicsSolver_->SolveXDirection(box, velocityCopy.x);
    }

    //Move the position by the measured distance.
    //位置按测量出的距离移动。
    transform2dComponent->SetPosition(box.center
                                      + WorldVector2D{horizontalSolveResult.distance, verticalSolveResult.distance});
}
