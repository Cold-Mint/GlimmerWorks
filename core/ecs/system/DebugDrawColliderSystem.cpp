/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#if  !defined(NDEBUG)
#include "DebugDrawColliderSystem.h"

#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/GraphicsContext.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/ColliderComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/gpu/RenderQueue.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/PreloadColors.h"
#include "core/world/TileCollision.h"
#include "core/world/WorldContext.h"

glimmer::DebugDrawColliderSystem::DebugDrawColliderSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_RIGID_BODY_2D);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_CAMERA);
    if (const AppContext *appContext = worldContext->GetAppContext(); appContext != nullptr) {
        if (const GraphicsContext *graphicsContext = appContext->GetGraphicsContext(); graphicsContext != nullptr) {
            preloadColors_ = graphicsContext->GetPreloadColors();
        }
    }
    Init();
}

bool glimmer::DebugDrawColliderSystem::CanActive() const {
    return displayCollisionBox_;
}

void glimmer::DebugDrawColliderSystem::OnConfigChanged(const Config *config) {
    displayCollisionBox_ = config->debug.displayCollisionBox;
}

void glimmer::DebugDrawColliderSystem::OnWatchedComponentChanged(const GameComponentTypeMessage gameComponentType,
                                                                 const uint32_t count) {
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    const EntityManager *entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D) {
        if (cameraTransform2DComponent_ == nullptr) {
            cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
        }
        transform2dCount_ = count;
    }
    if (gameComponentType == COMPONENT_RIGID_BODY_2D) {
        colliderCount_ = count;
    }
    if (colliderCount_ > 0 && transform2dCount_ > 0) {
        entities_ = entityManager->GetEntityIDWithComponents({COMPONENT_RIGID_BODY_2D, COMPONENT_TRANSFORM_2D});
    }
}

void glimmer::DebugDrawColliderSystem::RenderEntityColliders(RenderQueue *queue) const {
    EntityManager *entityManager = GetEntityManager();
    for (const GameEntityID entity: entities_) {
        const auto *collider = entityManager->GetComponent<ColliderComponent>(entity);
        if (collider == nullptr || !collider->IsEnabled()) {
            continue;
        }
        const auto *transform = entityManager->GetComponent<Transform2DComponent>(entity);
        if (transform == nullptr) {
            continue;
        }
        const WorldVector2D &position = transform->GetRenderPosition();
        if (!cameraComponent_->IsPointInViewport(cameraTransform2DComponent_->GetRenderPosition(), position)) {
            continue;
        }
        const float zoom = cameraComponent_->GetZoom();
        const float pixelW = collider->GetHalfWidth() * 2.0F * zoom;
        const float pixelH = collider->GetHalfHeight() * 2.0F * zoom;
        const WorldVector2D minCorner{
            position.x - collider->GetHalfWidth(),
            position.y - collider->GetHalfHeight()
        };
        const ScreenVector2D screenPos = CoordinateTransformer::WorldToScreen(
            cameraTransform2DComponent_->GetRenderPosition(), minCorner,
            cameraComponent_->GetSize(), zoom);
        const SDL_FRect rect{screenPos.x, screenPos.y - pixelH, pixelW, pixelH};

        const SDL_Color borderColor{
            preloadColors_->debugColor.collisionBoxBorderColor.r,
            preloadColors_->debugColor.collisionBoxBorderColor.g,
            preloadColors_->debugColor.collisionBoxBorderColor.b,
            preloadColors_->debugColor.collisionBoxBorderColor.a
        };
        const SDL_Color fullColor{
            preloadColors_->debugColor.collisionBoxFullColor.r,
            preloadColors_->debugColor.collisionBoxFullColor.g,
            preloadColors_->debugColor.collisionBoxFullColor.b,
            preloadColors_->debugColor.collisionBoxFullColor.a
        };
        queue->FillRect(RenderLayer::Debug, 0.0F, &rect, fullColor);
        queue->DrawRect(RenderLayer::Debug, 0.0F, &rect, borderColor);
    }
}

void glimmer::DebugDrawColliderSystem::RenderSolidTiles(RenderQueue *queue) const {
    const WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const SDL_FRect viewportRect = CoordinateTransformer::GetViewportRect(
        cameraTransform2DComponent_->GetRenderPosition(), cameraComponent_->GetSize(),
        cameraComponent_->GetZoom());
    const TileVector2D minTile = CoordinateTransformer::WorldToTile(
        WorldVector2D{viewportRect.x, viewportRect.y});
    const TileVector2D maxTile = CoordinateTransformer::WorldToTile(
        WorldVector2D{viewportRect.x + viewportRect.w, viewportRect.y + viewportRect.h});

    const float zoom = cameraComponent_->GetZoom();
    const float pixelSize = TILE_SIZE * zoom;
    const SDL_Color tileColor{
        preloadColors_->debugColor.collisionBoxTileColor.r,
        preloadColors_->debugColor.collisionBoxTileColor.g,
        preloadColors_->debugColor.collisionBoxTileColor.b,
        preloadColors_->debugColor.collisionBoxTileColor.a
    };

    for (int ty = minTile.y; ty <= maxTile.y; ++ty) {
        for (int tx = minTile.x; tx <= maxTile.x; ++tx) {
            if (!TileCollision::IsSolidTile(worldContext, TileVector2D{tx, ty})) {
                continue;
            }
            const WorldVector2D minCorner{
                static_cast<float>(tx * TILE_SIZE - HALF_TILE_SIZE),
                static_cast<float>(ty * TILE_SIZE - HALF_TILE_SIZE)
            };
            const ScreenVector2D screenPos = CoordinateTransformer::WorldToScreen(
                cameraTransform2DComponent_->GetRenderPosition(), minCorner,
                cameraComponent_->GetSize(), zoom);
            const SDL_FRect rect{screenPos.x, screenPos.y - pixelSize, pixelSize, pixelSize};
            queue->DrawRect(RenderLayer::Debug, 0.0F, &rect, tileColor);
        }
    }
}

void glimmer::DebugDrawColliderSystem::Render(RenderQueue *queue) {
    if (!displayCollisionBox_ || cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr ||
        preloadColors_ == nullptr) {
        return;
    }
    RenderEntityColliders(queue);
    RenderSolidTiles(queue);
}

uint8_t glimmer::DebugDrawColliderSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_DEBUG_COLLIDER;
}

glimmer::GameSystemType glimmer::DebugDrawColliderSystem::GetGameSystemType() const {
    return GameSystemType::DebugDrawColliderSystem;
}

#endif
