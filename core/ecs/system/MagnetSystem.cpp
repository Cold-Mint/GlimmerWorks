//
// Created by coldmint on 2025/12/22.
//

#include "MagnetSystem.h"

#include "../../world/WorldContext.h"
#include "../component/RigidBody2DComponent.h"
#include "../component/DroppedItemComponent.h"
#include "include/tweeny.h"


void glimmer::MagnetSystem::Update(const float delta) {
    auto magnets =
            worldContext_->GetEntitiesWithComponents<MagnetComponent, Transform2DComponent>();

    auto magnetics =
            worldContext_->GetEntitiesWithComponents<MagneticComponent, Transform2DComponent>();

    for (auto magnetEntity: magnets) {
        auto *magnet = worldContext_->GetComponent<MagnetComponent>(magnetEntity->GetID());
        auto *magnetTransform =
                worldContext_->GetComponent<Transform2DComponent>(magnetEntity->GetID());

        WorldVector2D magnetPos = magnetTransform->GetPosition();

        for (auto magneticEntity: magnetics) {
            auto *magnetic =
                    worldContext_->GetComponent<MagneticComponent>(magneticEntity->GetID());
            auto *magneticTransform =
                    worldContext_->GetComponent<Transform2DComponent>(magneticEntity->GetID());
            if ((magnet->GetType() & magnetic->GetType()) == 0) {
                LogCat::d("The magnet and the target type are different.");
                continue;
            }

            const auto *droppedItem = worldContext_->GetComponent<DroppedItemComponent>(magneticEntity->GetID());
            if (droppedItem != nullptr && !droppedItem->CanBePickedUp()) {
                continue;
            }

            WorldVector2D magneticPos = magneticTransform->GetPosition();
            const float distance = (magnetPos - magneticPos).Length();

            if (distance > magnet->GetDetectionRadius()) {
                auto *rigidBody2DComponent =
                        worldContext_->GetComponent<RigidBody2DComponent>(magneticEntity->GetID());
                if (rigidBody2DComponent != nullptr) {
                    rigidBody2DComponent->Enable();
                }
                magnetic->SetTweening(false);
                magnet->RemoveEntity(magneticEntity->GetID());
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

            LogCat::d("magnet addEntity :", magnetEntity->GetID(), " ,distance=", distance);
            auto *rigidBody2DComponent =
                    worldContext_->GetComponent<RigidBody2DComponent>(magneticEntity->GetID());
            if (rigidBody2DComponent != nullptr) {
                rigidBody2DComponent->Disable();
            }

            if (distance <= magnet->GetAdsorptionRadius()) {
                LogCat::d("distance=", distance, ",AdsorptionRadius=", magnet->GetAdsorptionRadius());
                magnet->AddEntity(magneticEntity->GetID());
                continue;
            }

            LogCat::d("magnet Move :", magnetEntity->GetID(), " ,distance=", distance);
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
