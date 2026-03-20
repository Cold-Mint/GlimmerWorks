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
                worldContext_->GetComponent<Transform2DComponent>(rayComp->GetTransform2DEntity());
        if (transform2dComponent == nullptr || !rayComp->IsContinuous()) {
            continue;
        }
        rayComp->SetHit(false);
        rayComp->SetHitShape(b2_nullShapeId);
        const b2RayResult rayResult = b2World_CastRayClosest(
            worldContext_->GetWorldId(),
            Box2DUtils::ToMeters(transform2dComponent->GetPosition() + rayComp->GetOrigin()),
            Box2DUtils::ToMeters(rayComp->GetTranslation()),
            rayComp->GetFilter()
        );
        if (rayResult.hit) {
            rayComp->SetHit(true);
            rayComp->SetHitPoint(WorldVector2D{rayResult.point.x, rayResult.point.y});
            rayComp->SetHitNormal(WorldVector2D{rayResult.normal.x, rayResult.normal.y});
            rayComp->SetHitShape(rayResult.shapeId);
        }
    }
}

std::string glimmer::RayCast2DSystem::GetName() {
    return "glimmer.RayCast2DSystem";
}
