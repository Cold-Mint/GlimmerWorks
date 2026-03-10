//
// Created by coldmint on 2026/3/8.
//

#include "RayCast2DSystem.h"

#include "box2d/box2d.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/utils/Box2DUtils.h"
#include "core/world/WorldContext.h"

glimmer::RayCast2DSystem::RayCast2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<RayCast2DComponent>();
}

void glimmer::RayCast2DSystem::Update(float delta) {
    const std::vector<GameEntity::ID> rayCast2dEntity =
            worldContext_->GetEntityIDWithComponents<RayCast2DComponent>();
    for (const GameEntity::ID entity: rayCast2dEntity) {
        const auto rayComp =
                worldContext_->GetComponent<RayCast2DComponent>(entity);
        if (rayComp == nullptr) {
            continue;
        }
        const auto transform2dComponent =
                worldContext_->GetComponent<Transform2DComponent>(rayComp->transform2DEntity);
        if (transform2dComponent == nullptr || !rayComp->enableContinuous) {
            continue;
        }
        rayComp->hit = false;
        rayComp->hitShapeId = b2_nullShapeId;
        rayComp->hitEntity = 0;
        const b2RayResult rayResult = b2World_CastRayClosest(
            worldContext_->GetWorldId(), Box2DUtils::ToMeters(transform2dComponent->GetPosition() + rayComp->origin),
            Box2DUtils::ToMeters(rayComp->translation),
            rayComp->filter
        );
        if (rayResult.hit) {
            rayComp->hit = true;
            rayComp->hitPoint = WorldVector2D{rayResult.point.x, rayResult.point.y};
            rayComp->hitNormal = WorldVector2D{rayResult.normal.x, rayResult.normal.y};
            rayComp->hitShapeId = rayResult.shapeId;
        }
    }
}

std::string glimmer::RayCast2DSystem::GetName() {
    return "glimmer.RayCast2DSystem";
}
