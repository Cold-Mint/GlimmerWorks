//
// Created by Cold-Mint on 2025/10/29.
//

#include "PlayerControlSystem.h"

#include <random>

#include "../../log/LogCat.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/CameraComponent.h"
#include "../../utils/Box2DUtils.h"
#include "../component/RigidBody2DComponent.h"
#include "box2d/box2d.h"
#include "../../Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/HotBarComonent.h"
#include "core/ecs/component/ItemContainerComonent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/RayCast2DComponent.h"

glimmer::PlayerControlSystem::PlayerControlSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<PlayerComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::PlayerControlSystem::Update(const float delta) {
    auto player = worldContext_->GetPlayerEntity();
    const auto playerComponent = worldContext_->GetComponent<PlayerComponent>(player);
    if (playerComponent == nullptr) {
        return;
    }
    const auto rigidBody2DComponent = worldContext_->GetComponent<RigidBody2DComponent>(player);
    if (rigidBody2DComponent == nullptr) {
        return;
    }

    if (!rigidBody2DComponent->IsReady()) {
        return;
    }

    const b2BodyId bodyId = rigidBody2DComponent->GetBodyId();
    bool isGrounded = OnGround(playerComponent);
    const b2Vec2 currentVel = b2Body_GetLinearVelocity(bodyId);
    float targetAccX = playerComponent->movementAcceleration * playerComponent->horizontalInput;
    if (!isGrounded) {
        targetAccX *= playerComponent->airControlFactor;
    }
    b2MassData massData = b2Body_GetMassData(bodyId);
    float horizontalForce = massData.mass * targetAccX;
    if (playerComponent->jump && isGrounded) {
        b2Vec2 jumpImpulse = {0, massData.mass * PLAYER_JUMP_FORCE};
        b2Body_ApplyLinearImpulseToCenter(bodyId, jumpImpulse, true);
        playerComponent->jump = false;
    }
    if (playerComponent->horizontalInput == 0) {
        // 制动力系数：地面摩擦力大，空中摩擦力小
        const float brakeFactor = isGrounded ? 8.0F : 2.0F;
        // 反向制动力 = -当前速度 × 质量 × 制动系数（与运动方向相反）
        horizontalForce = -currentVel.x * massData.mass * brakeFactor;

        // 可选：速度极小值时直接清零，避免无限滑
        if (fabs(currentVel.x) < 0.1F) {
            b2Body_SetLinearVelocity(bodyId, {0.0F, currentVel.y});
            horizontalForce = 0.0F; // 停止施力
        }
    }
    b2Body_ApplyForceToCenter(bodyId, {horizontalForce, 0.0F}, true);
    if (std::abs(currentVel.x) > playerComponent->maxSpeed) {
        //The horizontal speed has exceeded the maximum speed.
        //水平方向速度，超过了最大速度。
        if (currentVel.x > 0) {
            b2Body_SetLinearVelocity(bodyId, {playerComponent->maxSpeed, currentVel.y});
        } else {
            b2Body_SetLinearVelocity(bodyId, {-playerComponent->maxSpeed, currentVel.y});
        }
    }

    float gravityForce = massData.mass * GRAVITY_SCALE;
    b2Body_ApplyForceToCenter(bodyId, {0, -gravityForce}, true);
    const auto hotBarEntity = worldContext_->GetHotBarEntity();
    if (WorldContext::IsEmptyEntityId(hotBarEntity)) {
        LogCat::d("PlayerControlSystem hotBarEntity null");
        return;
    }
    auto hotBarComp = worldContext_->GetComponent<HotBarComponent>(hotBarEntity);
    const auto *containerComp = worldContext_->GetComponent<ItemContainerComponent>(player);

    playerComponent->dropTimer += delta;
    if (playerComponent->dropPressed && playerComponent->dropTimer >= DROP_INTERVAL) {
        playerComponent->dropTimer -= DROP_INTERVAL;
        if (hotBarComp && containerComp) {
            auto itemContainer = containerComp->GetItemContainer();
            if (itemContainer != nullptr) {
                auto item = itemContainer->TakeItem(hotBarComp->GetSelectedSlot(), 1);
                if (item != nullptr) {
                    const GameEntity::ID droppedEntity = worldContext_->CreateEntity();
                    DroppedItemCreator droppedItemCreator{worldContext_};
                    droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
                    droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                                  worldContext_->GetCameraTransform2D()->
                                                                  GetPosition(), std::move(item), 2));
                }
            }
        }
        playerComponent->dropPressed = false;
    }

    if (playerComponent->mouseLeftDown && hotBarComp && containerComp) {
        if (const auto itemContainer = containerComp->GetItemContainer()) {
            if (Item *item = itemContainer->GetItem(hotBarComp->GetSelectedSlot())) {
                slipTimer_ += delta;
                std::unordered_set<std::string> popupAbility;
                const AbilityConfig &abilityConfig = item->GetAbilityConfig();

                if (slipTimer_ > 0.5F) {
                    slipTimer_ = 0.0F;
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    float fumbleChance = std::clamp(abilityConfig.fumbleProbability, 0.0F, 1.0F);
                    if (fumbleChance > 0.0F) {
                        std::uniform_real_distribution dist(0.0F, 1.0F);
                        float randomValue = dist(rng);

                        if (randomValue <= fumbleChance) {
                            const GameEntity::ID droppedEntity = worldContext_->CreateEntity();
                            DroppedItemCreator droppedItemCreator{worldContext_};
                            droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                                      DroppedItemCreator::GetResourceRef());
                            droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                                      DroppedItemCreator::GetEntityItemMessage(
                                                                          worldContext_->GetCameraTransform2D()->
                                                                          GetPosition(),
                                                                          std::move(itemContainer->TakeAllItem(
                                                                              hotBarComp->GetSelectedSlot())), 2));
                            return;
                        }
                    }
                }
                item->OnUse(worldContext_, player, abilityConfig, popupAbility);
            }
        }
    } else {
        slipTimer_ = 0.0F;
    }
}

bool glimmer::PlayerControlSystem::OnGround(const PlayerComponent *playerControlComponent) const {
    if (playerControlComponent->groundCheckRayEntityIds.empty()) {
        return false;
    }
    for (uint32_t rayCast2dItem: playerControlComponent->groundCheckRayEntityIds) {
        auto rayCast2DComponent = worldContext_->GetComponent<RayCast2DComponent>(rayCast2dItem);
        if (rayCast2DComponent == nullptr) {
            continue;
        }
        if (rayCast2DComponent->IsHit()) {
            return true;
        }
    }
    return false;
}

std::string glimmer::PlayerControlSystem::GetName() {
    return "glimmer.PlayerControlSystem";
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        const auto *camera = worldContext_->GetCameraComponent();
        const auto *cameraTransform = worldContext_->GetCameraTransform2D();
        if (camera != nullptr && cameraTransform != nullptr) {
            WorldVector2D worldPos = camera->GetWorldPosition(
                cameraTransform->GetPosition(),
                CameraVector2D(event.motion.x, event.motion.y)
            );
            auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
            for (auto &entity: tileLayerEntities) {
                auto *layer = worldContext_->GetComponent<TileLayerComponent>(entity);
                if (layer != nullptr && layer->GetTileLayerType() == TileLayerType::Main) {
                    layer->SetFocusPosition(TileLayerComponent::WorldToTile(worldPos));
                }
            }
            return true;
        }
        return false;
    }

    const GameEntity::ID playerEntity = worldContext_->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return false;
    }
    auto *playerComponent = worldContext_->GetComponent<PlayerComponent>(playerEntity);
    if (playerComponent == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        const auto hotBarComponent = worldContext_->GetComponent<HotBarComponent>(worldContext_->GetHotBarEntity());
        if (hotBarComponent == nullptr) {
            return false;
        }
        int current = hotBarComponent->GetSelectedSlot();
        if (event.wheel.y > 0) {
            current = (current - 1 + hotBarComponent->GetMaxSlot()) % hotBarComponent->GetMaxSlot();
        } else if (event.wheel.y < 0) {
            current = (current + 1) % hotBarComponent->GetMaxSlot();
        }
        hotBarComponent->SetSelectedSlot(current);
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        playerComponent->mouseLeftDown = true;
        slipTimer_ = 0.0F;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
        playerComponent->mouseLeftDown = false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        float horizontalInput = 0.0F;
        if (event.key.key == SDLK_A && pressed) {
            horizontalInput -= 1.0F;
        }
        if (event.key.key == SDLK_D && pressed) {
            horizontalInput += 1.0F;
        }
        playerComponent->horizontalInput = horizontalInput;
        if (event.key.key == SDLK_SPACE) {
            playerComponent->jump = pressed;
        }
        if (event.key.key == SDLK_Q) {
            playerComponent->dropPressed = pressed;
            if (!pressed) {
                playerComponent->dropTimer = 0.0F;
            }
        }
    }
    return false;
}
