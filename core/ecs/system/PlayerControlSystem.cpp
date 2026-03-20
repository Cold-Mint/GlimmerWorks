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
    const auto control = worldContext_->GetComponent<PlayerComponent>(player);
    const auto rigid = worldContext_->GetComponent<RigidBody2DComponent>(player);

    if (control == nullptr || rigid == nullptr || !rigid->IsReady()) {
        LogCat::d("PlayerControlSystem update null");
        return;
    }

    const b2BodyId bodyId = rigid->GetBodyId();
    bool isGrounded = OnGround(control);
    int shapeCount = b2Body_GetShapeCount(bodyId);
    b2ShapeId shapeIds[shapeCount];
    int shapeCountActual = b2Body_GetShapes(bodyId, shapeIds, shapeCount);
    for (int i = 0; i < shapeCountActual; i++) {
        b2Shape_SetFriction(shapeIds[i], isGrounded ? 0.8f : 0.0f); // 地面增加摩擦，空中无摩擦
    }

    // ========== 优化：平滑移动计算 ==========
    const b2Vec2 currentVel = b2Body_GetLinearVelocity(bodyId);
    float targetVX = 0.0F;
    if (control->moveLeft) {
        targetVX -= PLAYER_MOVE_SPEED_OPTIMIZED;
    }
    if (control->moveRight) {
        targetVX += PLAYER_MOVE_SPEED_OPTIMIZED;
    }

    // 空中移动衰减
    if (!isGrounded) {
        targetVX *= AIR_CONTROL_FACTOR;
    }

    // 平滑加速/减速（核心手感优化）
    float smoothVX = CalculateSmoothMoveSpeed(targetVX, currentVel.x, delta, isGrounded);

    // ========== 修复：跳跃逻辑 ==========
    float targetVY = currentVel.y;
    if (control->jump && isGrounded) {
        targetVY = PLAYER_JUMP_FORCE;
        control->jump = false; // 防止持续跳跃
        LogCat::d("Player jump success, grounded: %d", isGrounded);
    }

    // 重力应用（优化下落手感）
    if (!isGrounded) {
        targetVY -= GRAVITY_SCALE * delta;
    }

    // 设置最终速度
    b2Body_SetLinearVelocity(bodyId, {smoothVX, targetVY});

    // ========== 物品掉落逻辑（保留原有，修复输入重置） ==========
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
        // 修复：防止重复触发
        control->dropPressed = false;
    }

    // ========== 物品使用逻辑（保留原有，优化手滑判定） ==========
    if (control->mouseLeftDown && hotBarComp && containerComp) {
        if (const auto itemContainer = containerComp->GetItemContainer()) {
            if (Item *item = itemContainer->GetItem(hotBarComp->GetSelectedSlot())) {
                slipTimer += delta;
                std::unordered_set<std::string> popupAbility;
                const VariableConfig &variableConfig = item->GetVariableConfig();

                // 优化：手滑判定间隔缩短，更灵敏
                if (slipTimer > 0.5F) {
                    slipTimer = 0.0F; // 重置计时器，避免重复触发
                    std::random_device rd;
                    std::mt19937 rng(rd());
                    auto fumbleChanceVariable = variableConfig.FindVariable("fumbleChance");

                    if (fumbleChanceVariable != nullptr) {
                        float fumbleChance = fumbleChanceVariable->AsFloat();
                        fumbleChance = std::clamp(fumbleChance, 0.0F, 1.0F);

                        if (fumbleChance > 0.0F) {
                            std::uniform_real_distribution dist(0.0F, 1.0F);
                            float randomValue = dist(rng);

                            if (randomValue <= fumbleChance) {
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
    } else {
        // 修复：鼠标松开时重置手滑计时器
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

// ========== 新增：平滑移动计算（核心手感优化） ==========
float glimmer::PlayerControlSystem::CalculateSmoothMoveSpeed(float targetSpeed, float currentSpeed, float delta,
                                                             bool isGrounded) {
    float acceleration = isGrounded ? MOVE_ACCELERATION : MOVE_ACCELERATION * AIR_CONTROL_FACTOR;
    float deceleration = isGrounded ? MOVE_DECELERATION : MOVE_DECELERATION * AIR_CONTROL_FACTOR;

    if (targetSpeed != 0) {
        // 加速到目标速度
        currentSpeed = std::lerp(currentSpeed, targetSpeed, acceleration * delta);
    } else {
        // 无输入时减速到0
        currentSpeed = std::lerp(currentSpeed, 0, deceleration * delta);
        // 防止微小速度残留
        if (fabs(currentSpeed) < 0.01f) {
            currentSpeed = 0.0f;
        }
    }

    return currentSpeed;
}

std::string glimmer::PlayerControlSystem::GetName() {
    return "glimmer.PlayerControlSystem";
}

// ========== 修复：输入处理逻辑（防止按键被覆盖、重复触发） ==========
bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event &event) {
    auto *camera = worldContext_->GetCameraComponent();
    auto *cameraTransform = worldContext_->GetCameraTransform2D();

    // 滚轮切换快捷栏（保留原有）
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

    // 鼠标左键（保留原有，修复重置逻辑）
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        const auto entities = worldContext_->GetEntityIDWithComponents<PlayerComponent>();
        for (auto &entity: entities) {
            if (auto *control = worldContext_->GetComponent<PlayerComponent>(entity)) {
                control->mouseLeftDown = true;
                slipTimer = 0.0F; // 重置手滑计时器
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

    // 键盘输入（修复：移除return，防止按键覆盖）
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        const auto entities = worldContext_->GetEntityIDWithComponents<PlayerComponent>();
        for (auto &entity: entities) {
            auto control = worldContext_->GetComponent<PlayerComponent>(entity);
            if (control == nullptr) continue;

            switch (event.key.key) {
                case SDLK_A:
                    control->moveLeft = pressed;
                    LogCat::d("pressed A: %d", pressed);
                    break;
                case SDLK_D:
                    control->moveRight = pressed;
                    LogCat::d("pressed D: %d", pressed);
                    break;
                case SDLK_SPACE:
                    if (pressed) control->jump = true;
                    LogCat::d("pressed SPACE: %d", pressed);
                    break;
                case SDLK_Q:
                    control->dropPressed = pressed;
                    if (!pressed) control->dropTimer = 0.0f; // 松开时重置计时器
                    LogCat::d("pressed Q: %d", pressed);
                    break;
                default:
                    break;
            }
        }
    }
    return false;
}
