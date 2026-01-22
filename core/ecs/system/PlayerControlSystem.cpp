//
// Created by Cold-Mint on 2025/10/29.
//

#include "PlayerControlSystem.h"
#include "../../log/LogCat.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/CameraComponent.h"
#include "../../utils/Box2DUtils.h"
#include "../component/RigidBody2DComponent.h"
#include "box2d/box2d.h"
#include "../component/HotBarComonent.h"
#include "../../Constants.h"
#include "../component/ItemContainerComonent.h"


void glimmer::PlayerControlSystem::Update(const float delta) {
    const auto entities = worldContext_->GetEntityIDWithComponents<PlayerControlComponent, RigidBody2DComponent>();
    for (auto &entity: entities) {
        const auto control = worldContext_->GetComponent<PlayerControlComponent>(entity);
        const auto rigid = worldContext_->GetComponent<RigidBody2DComponent>(entity);
        if (control == nullptr || rigid == nullptr || !rigid->IsReady()) {
            continue;
        }

        const b2BodyId bodyId = rigid->GetBodyId();

        // Fix: Set friction to 0 to prevent sticking to walls when moving against them
        // 修复：将摩擦力设置为0，以防止移动时贴在墙上
        b2ShapeId shapeId;
        if (b2Body_GetShapes(bodyId, &shapeId, 1) > 0) {
            b2Shape_SetFriction(shapeId, 0.0F);
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
            return;
        }
        auto hotBarComp = worldContext_->GetComponent<HotBarComponent>(hotBarEntity);
        const auto *containerComp = worldContext_->GetComponent<ItemContainerComponent>(entity);
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
            auto itemContainer = containerComp->GetItemContainer();
            if (itemContainer) {
                Item *item = itemContainer->GetItem(hotBarComp->GetSelectedSlot());
                if (item) {
                    item->OnUse(appContext_, worldContext_, entity);
                }
            }
        }
    }
}


bool glimmer::PlayerControlSystem::onGround(const RigidBody2DComponent *rigid) const {
    b2Vec2 position = b2Body_GetPosition(rigid->GetBodyId());
    float width = Box2DUtils::ToMeters(rigid->GetWidth());
    float height = Box2DUtils::ToMeters(rigid->GetHeight());
    const b2Vec2 leftBottom = {position.x - width * 0.5F, position.y - height * 0.5F};
    const b2Vec2 rightBottom = {position.x + width * 0.5F, position.y - height * 0.5F};
    constexpr float rayLength = 0.1F;
    b2Vec2 translation = {0.0F, -rayLength};
    b2QueryFilter filter{};
    filter.categoryBits = 0xFFFF;
    filter.maskBits = 0xFFFF;
    struct RayContext {
        bool hit = false;
    };
    auto rayCallback = [](b2ShapeId, b2Vec2, b2Vec2, float, void *ctx) -> float {
        auto *context = static_cast<RayContext *>(ctx);
        context->hit = true;
        return 0.0F;
    };

    RayContext leftCtx;
    RayContext rightCtx;
    b2World_CastRay(worldContext_->GetWorldId(), leftBottom, translation, filter, rayCallback, &leftCtx);
    b2World_CastRay(worldContext_->GetWorldId(), rightBottom, translation, filter, rayCallback, &rightCtx);
    return leftCtx.hit || rightCtx.hit;
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
                    return true;
                case SDLK_D: control->moveRight = pressed;
                    return true;
                case SDLK_SPACE:
                    control->jump = pressed;
                    return true;
                case SDLK_Q:
                    control->dropPressed = pressed;
                    return true;
                default:
                    return false;
            }
        }
    }

    return false;
}
