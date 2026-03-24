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
    float targetVX = 0.0F;
    if (playerComponent->moveLeft) {
        targetVX -= playerComponent->movementSpeed * TILE_SIZE;
    }
    if (playerComponent->moveRight) {
        targetVX += playerComponent->movementSpeed * TILE_SIZE;
    }
    if (!isGrounded) {
        targetVX *= AIR_CONTROL_FACTOR;
    }
    float smoothVX = CalculateSmoothMoveSpeed(targetVX, currentVel.x, delta, isGrounded);
    float targetVY = currentVel.y;
    if (playerComponent->jump && isGrounded) {
        targetVY = PLAYER_JUMP_FORCE;
        playerComponent->jump = false;
    }
    if (!isGrounded) {
        targetVY -= GRAVITY_SCALE * delta;
    }

    b2Body_SetLinearVelocity(bodyId, {smoothVX, targetVY});

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
                slipTimer += delta;
                std::unordered_set<std::string> popupAbility;
                const AbilityConfig &abilityConfig = item->GetAbilityConfig();

                if (slipTimer > 0.5F) {
                    slipTimer = 0.0F;
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
        slipTimer = 0.0F;
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

float glimmer::PlayerControlSystem::CalculateSmoothMoveSpeed(const float targetSpeed, float currentSpeed,
                                                             const float delta,
                                                             const bool isGrounded) const {
    const float acceleration = isGrounded ? MOVE_ACCELERATION : MOVE_ACCELERATION * AIR_CONTROL_FACTOR;
    const float deceleration = isGrounded ? MOVE_DECELERATION : MOVE_DECELERATION * AIR_CONTROL_FACTOR;

    if (targetSpeed != 0) {
        currentSpeed = std::lerp(currentSpeed, targetSpeed, acceleration * delta);
    } else {
        currentSpeed = std::lerp(currentSpeed, 0, deceleration * delta);
        if (fabs(currentSpeed) < 0.01f) {
            currentSpeed = 0.0f;
        }
    }
    return currentSpeed;
}

std::string glimmer::PlayerControlSystem::GetName() {
    return "glimmer.PlayerControlSystem";
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event &event) {
    auto *camera = worldContext_->GetCameraComponent();
    auto *cameraTransform = worldContext_->GetCameraTransform2D();
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        const auto hotBarComponent = worldContext_->GetComponent<HotBarComponent>(worldContext_->GetHotBarEntity());
        if (hotBarComponent == nullptr) return false;

        int current = hotBarComponent->GetSelectedSlot();
        if (event.wheel.y > 0) {
            current = (current - 1 + hotBarComponent->GetMaxSlot()) % hotBarComponent->GetMaxSlot();
        } else if (event.wheel.y < 0) {
            current = (current + 1) % hotBarComponent->GetMaxSlot();
        }
        hotBarComponent->SetSelectedSlot(current);
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION && camera && cameraTransform) {
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
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        const auto entities = worldContext_->GetEntityIDWithComponents<PlayerComponent>();
        for (auto &entity: entities) {
            if (auto *control = worldContext_->GetComponent<PlayerComponent>(entity)) {
                control->mouseLeftDown = true;
                slipTimer = 0.0F;
            }
        }
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
        const auto entities = worldContext_->GetEntityIDWithComponents<PlayerComponent>();
        for (auto &entity: entities) {
            if (auto *control = worldContext_->GetComponent<PlayerComponent>(entity)) {
                control->mouseLeftDown = false;
            }
        }
    }
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        const auto entities = worldContext_->GetEntityIDWithComponents<PlayerComponent>();
        for (auto &entity: entities) {
            auto control = worldContext_->GetComponent<PlayerComponent>(entity);
            if (control == nullptr) continue;

            switch (event.key.key) {
                case SDLK_A:
                    control->moveLeft = pressed;
                    break;
                case SDLK_D:
                    control->moveRight = pressed;
                    break;
                case SDLK_SPACE:
                    if (pressed) control->jump = true;
                    break;
                case SDLK_Q:
                    control->dropPressed = pressed;
                    if (!pressed) control->dropTimer = 0.0f;
                    break;
                default:
                    break;
            }
        }
    }
    return false;
}
