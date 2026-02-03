//
// Created by coldmint on 2025/12/22.
//

#include "MagnetSystem.h"

#include "../../world/WorldContext.h"
#include "../component/RigidBody2DComponent.h"
#include "../component/DroppedItemComponent.h"
#include "../component/MagnetComponent.h"
#include "../component/MagneticComponent.h"
#include "../component/Transform2DComponent.h"
#include "tweeny.h"


glimmer::MagnetSystem::MagnetSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<Transform2DComponent>();
    RequireComponent<MagnetComponent>();
    RequireComponent<MagneticComponent>();
}

void glimmer::MagnetSystem::Update(const float delta) {
    auto magnets =
            worldContext_->GetEntityIDWithComponents<MagnetComponent, Transform2DComponent>();

    auto magnetics =
            worldContext_->GetEntityIDWithComponents<MagneticComponent, Transform2DComponent>();

    for (auto magnetEntity: magnets) {
        auto *magnet = worldContext_->GetComponent<MagnetComponent>(magnetEntity);
        auto *magnetTransform =
                worldContext_->GetComponent<Transform2DComponent>(magnetEntity);

        WorldVector2D magnetPos = magnetTransform->GetPosition();

        for (auto magneticEntity: magnetics) {
            auto *magnetic =
                    worldContext_->GetComponent<MagneticComponent>(magneticEntity);
            auto *magneticTransform =
                    worldContext_->GetComponent<Transform2DComponent>(magneticEntity);
            if ((magnet->GetType() & magnetic->GetType()) == 0) {
                LogCat::d("The magnet and the target type are different.");
                continue;
            }

            const auto *droppedItem = worldContext_->GetComponent<DroppedItemComponent>(magneticEntity);
            if (droppedItem != nullptr && !droppedItem->CanBePickedUp()) {
                continue;
            }

            WorldVector2D magneticPos = magneticTransform->GetPosition();
            const float distance = (magnetPos - magneticPos).Length();

            if (distance > magnet->GetDetectionRadius()) {
                auto *rigidBody2DComponent =
                        worldContext_->GetComponent<RigidBody2DComponent>(magneticEntity);
                if (rigidBody2DComponent != nullptr) {
                    rigidBody2DComponent->Enable();
                }
                magnetic->SetTweening(false);
                magnet->RemoveEntity(magneticEntity);
                continue;
            }

            if (!magnetic->IsTweening()) {
                magnetic->SetTweening(true);

                magnetic->SetStartPos(magneticPos);

                magnetic->SetTween(tweeny::from(0.0F)
                    .to(1.0F)
                    .during(350.0F) // 毫秒
                    .via(tweeny::easing::cubicIn)); // 吸力感核心
            }

            LogCat::d("magnet addEntity :", magnetEntity, " ,distance=", distance);
            auto *rigidBody2DComponent =
                    worldContext_->GetComponent<RigidBody2DComponent>(magneticEntity);
            if (rigidBody2DComponent != nullptr) {
                rigidBody2DComponent->Disable();
            }

            if (distance <= magnet->GetAdsorptionRadius()) {
                LogCat::d("distance=", distance, ",AdsorptionRadius=", magnet->GetAdsorptionRadius());
                magnet->AddEntity(magneticEntity);
                continue;
            }

            LogCat::d("magnet Move :", magnetEntity, " ,distance=", distance);
            if (magnetic->IsTweening()) {
                magnetic->GetTween().step(static_cast<int>(delta * 1000));

                float t = magnetic->GetTween().peek();

                const WorldVector2D pos =
                        magnetic->GetStartPos() * (1.0F - t) +
                        magnetPos * t;

                magneticTransform->SetPosition(pos);

                if (magnetic->GetTween().progress() >= 1.0F) {
                    magneticTransform->SetPosition(magnetPos);
                    magnetic->SetTweening(false);
                }
            }
        }
    }
}

std::string glimmer::MagnetSystem::GetName() {
    return "glimmer.MagnetSystem";
}
