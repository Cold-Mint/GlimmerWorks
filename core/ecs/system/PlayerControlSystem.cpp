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
#include "../component/DroppedItemComponent.h"
#include "../../Constants.h"
#include "../../inventory/TileItem.h"
#include "../../world/ChunkPhysicsHelper.h"


void glimmer::PlayerControlSystem::Update(const float delta) {
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent, RigidBody2DComponent>();
    for (auto &entity: entities) {
        const auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
        const auto rigid = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
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
        if (!control->mouseLeftDown) {
            continue;
        }
        auto tileLayerEntitys = worldContext_->GetEntitiesWithComponents<
            TileLayerComponent, Transform2DComponent>();
        for (auto &gameEntity: tileLayerEntitys) {
            auto *tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(
                gameEntity->GetID());
            auto *tileLayerTransform2D = worldContext_->GetComponent<Transform2DComponent>(
                gameEntity->GetID());
            if (tileLayerComponent == nullptr || tileLayerTransform2D == nullptr) {
                continue;
            }
            if (tileLayerComponent->GetTileLayerType() == TileLayerType::Main) {
                TileVector2D tileVector2D = tileLayerComponent->GetFocusPosition();
                const Tile *tile = tileLayerComponent->GetTile(
                    tileVector2D);
                if (tile == nullptr) {
                    continue;
                }
                if (!tile->breakable) {
                    continue;
                }
                auto oldTile = tileLayerComponent->ReplaceTile(tileVector2D,
                                                               Tile::FromResourceRef(
                                                                   appContext_,
                                                                   appContext_->GetTileManager()->GetAir()));
                GameEntity *droppedEntity = worldContext_->CreateEntity();
                auto *transform2dComponent = worldContext_->AddComponent<
                    Transform2DComponent>(droppedEntity);
                transform2dComponent->SetPosition(
                    TileLayerComponent::TileToWorld(tileLayerTransform2D->GetPosition(), tileVector2D));
                worldContext_->AddComponent<DroppedItemComponent>(
                    droppedEntity, std::make_unique<TileItem>(std::move(oldTile))
                );
                const auto rigidBody2DComponent = worldContext_->AddComponent<RigidBody2DComponent>(
                    droppedEntity);
                rigidBody2DComponent->SetBodyType(b2_dynamicBody);
                rigidBody2DComponent->SetWidth(DROPPED_ITEM_SIZE);
                rigidBody2DComponent->SetHeight(DROPPED_ITEM_SIZE);
                rigidBody2DComponent->CreateBody(worldContext_->GetWorldId(),
                                                 Box2DUtils::ToMeters(
                                                     transform2dComponent->GetPosition()));
                const auto chunk = Chunk::GetChunkByTileVector2D(worldContext_->GetAllChunks(), tileVector2D);
                if (chunk == nullptr) {
                    continue;
                }
                ChunkPhysicsHelper::DetachPhysicsBodyToChunk(chunk);
                ChunkPhysicsHelper::AttachPhysicsBodyToChunk(worldContext_->GetWorldId(),
                                                             tileLayerTransform2D->GetPosition(),
                                                             chunk);
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

    // HotBar Switch
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        const auto entities = worldContext_->GetEntitiesWithComponents<
            PlayerControlComponent, HotBarComponent>();
        for (auto &entity: entities) {
            if (const auto hotBar = worldContext_->GetComponent<HotBarComponent>(entity->GetID())) {
                int current = hotBar->GetSelectedSlot();
                if (event.wheel.y > 0) {
                    current--;
                } else if (event.wheel.y < 0) {
                    current++;
                }
                hotBar->SetSelectedSlot(current);
            }
        }
        return false;
    }

    if (event.type == SDL_EVENT_MOUSE_MOTION && camera && cameraTransform) {
        //Set the tile for the current mouse focus.
        //设置当前位于鼠标焦点的瓦片。
        float mouseX = event.motion.x;
        float mouseY = event.motion.y;

        WorldVector2D worldPos = camera->GetWorldPosition(
            cameraTransform->GetPosition(),
            CameraVector2D(mouseX, mouseY)
        );

        auto tileLayerEntities = worldContext_->GetEntitiesWithComponents<
            TileLayerComponent, Transform2DComponent>();

        for (auto &entity: tileLayerEntities) {
            auto *layer = worldContext_->GetComponent<TileLayerComponent>(entity->GetID());
            auto *transform = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
            if (!layer || !transform) continue;

            if (layer->GetTileLayerType() != TileLayerType::Main) continue;
            layer->SetFocusPosition(TileLayerComponent::WorldToTile(
                transform->GetPosition(),
                worldPos
            ));
        }
    }


    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        const auto entities =
                worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();

        for (auto &entity: entities) {
            if (auto *control =
                    worldContext_->GetComponent<PlayerControlComponent>(entity->GetID())) {
                control->mouseLeftDown = true;
            }
        }
        return false;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
        event.button.button == SDL_BUTTON_LEFT) {
        const auto entities =
                worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();

        for (auto &entity: entities) {
            if (auto *control =
                    worldContext_->GetComponent<PlayerControlComponent>(entity->GetID())) {
                control->mouseLeftDown = false;
            }
        }
        return false;
    }

    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        for (auto &entity: entities) {
            auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
            if (control == nullptr) {
                continue;
            }
            switch (event.key.key) {
                case SDLK_A: control->moveLeft = pressed;
                    return true;
                case SDLK_D: control->moveRight = pressed;
                    return true;
                case SDLK_SPACE:
                    if (pressed) {
                        control->jump = true;
                    }
                    return true;

                default:
                    return false;
            }
        }
    }

    return false;
}
