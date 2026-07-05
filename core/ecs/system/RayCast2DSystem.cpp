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
#include "RayCast2DSystem.h"

#include "box2d/box2d.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/utils/Box2DUtils.h"
#include "core/world/WorldContext.h"

void glimmer::RayCast2DSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_RAY_CAST_2D)
    {
        entities_ = entityManager->GetEntityIDWithComponents({COMPONENT_RAY_CAST_2D});
    }
}

glimmer::RayCast2DSystem::RayCast2DSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_RAY_CAST_2D);
    Init();
}

void glimmer::RayCast2DSystem::Update(float delta)
{
    const WorldContext* worldContext = GetWorldContext();
    EntityManager* entityManager = GetEntityManager();
    if (worldContext == nullptr)
    {
        return;
    }
    if (entityManager == nullptr)
    {
        return;
    }
    for (const uint32_t entity : entities_)
    {
        const auto rayComp =
            entityManager->GetComponent<RayCast2DComponent>(entity);
        if (rayComp == nullptr)
        {
            continue;
        }
        const auto transform2dComponent =
            entityManager->GetComponent<Transform2DComponent>(rayComp->GetTransform2DEntity());
        if (transform2dComponent == nullptr)
        {
            continue;
        }
        rayComp->SetHit(false);
        rayComp->SetHitShape(b2_nullShapeId);
        const b2RayResult rayResult = b2World_CastRayClosest(
            worldContext->GetWorldId(),
            Box2DUtils::ToMeters(transform2dComponent->GetPosition() + rayComp->GetOrigin()),
            Box2DUtils::ToMeters(rayComp->GetTranslation()),
            rayComp->GetFilter()
        );
        if (rayResult.hit)
        {
            rayComp->SetHit(true);
            rayComp->SetHitPoint(WorldVector2D{rayResult.point.x, rayResult.point.y});
            rayComp->SetHitNormal(WorldVector2D{rayResult.normal.x, rayResult.normal.y});
            rayComp->SetHitShape(rayResult.shapeId);
        }
    }
}

glimmer::GameSystemType glimmer::RayCast2DSystem::GetGameSystemType() const
{
    return GameSystemType::RayCast2DSystem;
}
