//
// Created by Cold-Mint on 2025/12/22.
//

#include "MagnetSystem.h"

#include "../../world/WorldContext.h"
#include "../component/RigidBody2DComponent.h"
#include "../component/DroppedItemComponent.h"
#include "../component/MagnetComponent.h"
#include "../component/MagneticComponent.h"
#include "../component/Transform2DComponent.h"
#include "box2d/box2d.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/utils/Box2DUtils.h"


glimmer::MagnetSystem::MagnetSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<Transform2DComponent>();
    RequireComponent<MagnetComponent>();
    RequireComponent<MagneticComponent>();
    RequireComponent<RigidBody2DComponent>();
    RequireComponent<RayCast2DComponent>();
}

void glimmer::MagnetSystem::Update(const float delta) {
    if (worldContext_ == nullptr) {
        return;
    }
    //magnets
    //磁铁
    auto magnetEntityList =
            worldContext_->GetEntityIDWithComponents<MagnetComponent, Transform2DComponent>();

    if (magnetEntityList.empty()) {
        return;
    }

    //magnetics
    //磁性材料
    auto magneticList =
            worldContext_->GetEntityIDWithComponents<MagneticComponent, Transform2DComponent, RigidBody2DComponent,
                RayCast2DComponent>();

    if (magneticList.empty()) {
        return;
    }

    for (auto magnetEntity: magnetEntityList) {
        auto *magnet = worldContext_->GetComponent<MagnetComponent>(magnetEntity);
        if (magnet == nullptr) {
            continue;
        }
        auto *magnetTransform =
                worldContext_->GetComponent<Transform2DComponent>(magnetEntity);
        if (magnetTransform == nullptr) {
            continue;
        }
        const WorldVector2D magnetPos = magnetTransform->GetPosition();

        for (auto magneticEntity: magneticList) {
            auto *magneticTransform =
                    worldContext_->GetComponent<Transform2DComponent>(magneticEntity);
            if (magneticTransform == nullptr) {
                continue;
            }
            auto *magnetic =
                    worldContext_->GetComponent<MagneticComponent>(magneticEntity);
            if (magnetic == nullptr) {
                continue;
            }
            auto *rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(magneticEntity);
            if (rigidBody2DComponent == nullptr || !rigidBody2DComponent->IsReady() || !rigidBody2DComponent->
                IsEnabled()) {
                continue;
            }

            auto *rayCast2DComponent = worldContext_->GetComponent<RayCast2DComponent>(magneticEntity);
            if (rayCast2DComponent == nullptr) {
                continue;
            }

            if ((magnet->GetType() & magnetic->GetType()) == 0) {
                //No overlapping attraction areas.
                //没有重叠的吸引位。
                continue;
            }
            const auto *droppedItem = worldContext_->GetComponent<DroppedItemComponent>(magneticEntity);
            if (droppedItem == nullptr) {
                continue;
            }
            if (!droppedItem->CanBePickedUp()) {
                continue;
            }

            WorldVector2D magneticPos = magneticTransform->GetPosition();
            const WorldVector2D distanceVector = magnetPos - magneticPos;
            const float distance = distanceVector.Length();
            if (distance > magnet->GetDetectionRadius()) {
                magnet->RemoveEntity(magneticEntity);
                continue;
            }
            rayCast2DComponent->SetTransform(distanceVector);
            if (rayCast2DComponent->IsHit()) {
                //It was blocked by an obstacle.
                //被障碍物遮挡了。
                continue;
            }
            b2MassData massData = b2Body_GetMassData(rigidBody2DComponent->GetBodyId());
            WorldVector2D force = distanceVector.Normalized() * massData.mass * 80;
            b2Body_ApplyForceToCenter(rigidBody2DComponent->GetBodyId(),
                                      {force.x, force.y}, true);
            if (distance <= magnet->GetAdsorptionRadius()) {
                magnet->AddEntity(magneticEntity);
            }
        }
    }
}

std::string glimmer::MagnetSystem::GetName() {
    return "glimmer.MagnetSystem";
}
