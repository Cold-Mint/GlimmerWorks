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
#include "../component/HotBarComonent.h"
#include "../../Constants.h"
#include "../component/ItemContainerComonent.h"
#include "core/ecs/component/PlayerControlComponent.h"


glimmer::PlayerControlSystem::PlayerControlSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<PlayerControlComponent>();
    RequireComponent<Transform2DComponent>();
}

void glimmer::PlayerControlSystem::Update(const float delta) {
    auto player = worldContext_->GetPlayerEntity();
    const auto control = worldContext_->GetComponent<PlayerControlComponent>(player);
    const auto rigid = worldContext_->GetComponent<RigidBody2DComponent>(player);
    if (control == nullptr || rigid == nullptr || !rigid->IsReady()) {
        LogCat::d("PlayerControlSystem update null");
        return;
    }

    const b2BodyId bodyId = rigid->GetBodyId();
    int shapeCount = b2Body_GetShapeCount(bodyId);
    b2ShapeId shapeIds[shapeCount];
    int shapeCountActual = b2Body_GetShapes(bodyId, shapeIds, shapeCount);
    for (int i = 0; i < shapeCountActual; i++) {
        b2Shape_SetFriction(shapeIds[i], 0);
    }

    const b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
    float vx = 0.0F;
    if (control->moveLeft) {
        vx -= PLAYER_MOVE_SPEED;
    }
    if (control->moveRight) {
        vx += PLAYER_MOVE_SPEED;
    }
    float vy = vel.y;
    if (control->jump) {
        bool ground = onGround(rigid);
        if (ground) {
            vy = 10.0F;
        }
        control->jump = false;
    }

    b2Body_SetLinearVelocity(bodyId, {vx, vy});
    const auto hotBarEntity = worldContext_->GetHotBarEntity();
    if (WorldContext::IsEmptyEntityId(hotBarEntity)) {
        LogCat::d("PlayerControlSystem hotBarEntity null");
        return;
    }
    auto hotBarComp = worldContext_->GetComponent<HotBarComponent>(hotBarEntity);
    const auto *containerComp = worldContext_->GetComponent<ItemContainerComponent>(player);
    control->dropTimer += delta;
    if (control->dropPressed && control->dropTimer >= DROP_INTERVAL) {
        control->dropTimer -= DROP_INTERVAL;
        if (hotBarComp && containerComp) {
            auto itemContainer = containerComp->GetItemContainer();
            if (itemContainer != nullptr) {
                auto item = itemContainer->TakeItem(hotBarComp->GetSelectedSlot(), 1);
                if (item != nullptr) {
                    worldContext_->CreateDroppedItemEntity(
                        std::move(item),
                        worldContext_->GetCameraTransform2D()->GetPosition(),
                        2
                    );
                }
            }
        }
    }

    if (control->mouseLeftDown && hotBarComp && containerComp) {
        if (const auto itemContainer = containerComp->GetItemContainer()) {
            if (Item *item = itemContainer->GetItem(hotBarComp->GetSelectedSlot())) {
                slipTimer += delta;
                std::unordered_set<std::string> popupAbility;
                const VariableConfig &variableConfig = item->GetVariableConfig();
                if (slipTimer > 1.0F) {
                    slipTimer -= 1.0F;
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    auto fumbleChanceVariable = variableConfig.FindVariable("fumbleChance");
                    if (fumbleChanceVariable != nullptr) {
                        //With a hand-slipping effect
                        //带有手滑效果
                        float fumbleChance = fumbleChanceVariable->AsFloat();
                        fumbleChance = std::clamp(fumbleChance, 0.0F, 1.0F);
                        if (fumbleChance > 0.0F) {
                            std::uniform_real_distribution dist(0.0F, 1.0F);
                            float randomValue = dist(rng);
                            if (randomValue <= fumbleChance) {
                                //Triggered hand slip
                                //触发手滑
                                worldContext_->CreateDroppedItemEntity(
                                    std::move(itemContainer->TakeAllItem(hotBarComp->GetSelectedSlot())),
                                    worldContext_->GetCameraTransform2D()->GetPosition(),
                                    2
                                );
                                return;
                            }
                        }
                    }
                }
                item->OnUse(worldContext_, player, variableConfig, popupAbility);
            }
        }
    }
}


bool glimmer::PlayerControlSystem::onGround(const RigidBody2DComponent *rigid) const {
    return false;
}


std::string glimmer::PlayerControlSystem::GetName() {
    return "glimmer.PlayerControlSystem";
}


bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event &event) {
    auto *camera = worldContext_->GetCameraComponent();
    auto *cameraTransform = worldContext_->GetCameraTransform2D();

    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        const auto hotBarComponent = worldContext_->GetComponent<HotBarComponent>(worldContext_->GetHotBarEntity());
        int current = hotBarComponent->GetSelectedSlot();
        if (event.wheel.y > 0) {
            current--;
        } else if (event.wheel.y < 0) {
            current++;
        }
        hotBarComponent->SetSelectedSlot(current);
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION && camera && cameraTransform) {
        //Set the tile for the current mouse focus.
        //设置当前位于鼠标焦点的瓦片。
        WorldVector2D worldPos = camera->GetWorldPosition(
            cameraTransform->GetPosition(),
            CameraVector2D(event.motion.x, event.motion.y)
        );
        auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<
            TileLayerComponent>();
        for (auto &entity: tileLayerEntities) {
            auto *layer = worldContext_->GetComponent<TileLayerComponent>(entity);
            if (layer == nullptr) {
                continue;
            }
            if (layer->GetTileLayerType() == TileLayerType::Main) {
                layer->SetFocusPosition(TileLayerComponent::WorldToTile(
                    worldPos
                ));
            }
        }
    }


    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        const auto entities =
                worldContext_->GetEntityIDWithComponents<PlayerControlComponent>();

        for (auto &entity: entities) {
            if (auto *control =
                    worldContext_->GetComponent<PlayerControlComponent>(entity)) {
                control->mouseLeftDown = true;
            }
        }
        return false;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
        event.button.button == SDL_BUTTON_LEFT) {
        const auto entities =
                worldContext_->GetEntityIDWithComponents<PlayerControlComponent>();

        for (auto &entity: entities) {
            if (auto *control =
                    worldContext_->GetComponent<PlayerControlComponent>(entity)) {
                control->mouseLeftDown = false;
            }
        }
        return false;
    }

    const auto entities = worldContext_->GetEntityIDWithComponents<PlayerControlComponent>();
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        for (auto &entity: entities) {
            auto control = worldContext_->GetComponent<PlayerControlComponent>(entity);
            if (control == nullptr) {
                continue;
            }
            switch (event.key.key) {
                case SDLK_A: control->moveLeft = pressed;
                    LogCat::d("pressed A");
                    return true;
                case SDLK_D: control->moveRight = pressed;
                    LogCat::d("pressed D");
                    return true;
                case SDLK_SPACE:
                    control->jump = pressed;
                    LogCat::d("pressed SPACE");
                    return true;
                case SDLK_Q:
                    control->dropPressed = pressed;
                    LogCat::d("pressed Q");
                    return true;
                default:
                    return false;
            }
        }
    }

    return false;
}
