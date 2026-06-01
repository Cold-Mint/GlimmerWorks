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
#include "DroppedItemSystem.h"

#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/DroppedItemComponent.h"
#include "../component/Transform2DComponent.h"


glimmer::DroppedItemSystem::DroppedItemSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent(COMPONENT_TRANSFORM_2D);
    RequireComponent(COMPONENT_DROPPED_ITEM);
}

uint8_t glimmer::DroppedItemSystem::GetRenderOrder() {
    return RENDER_ORDER_DROPPED_ITEM;
}

void glimmer::DroppedItemSystem::Update(float delta) {
    auto entity = worldContext_->GetEntityIDWithComponents<DroppedItemComponent, Transform2DComponent>();
    for (const auto &gameEntity: entity) {
        auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(
            gameEntity);
        auto *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(gameEntity);
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        if (droppedItemComponent->IsExpired()) {
            worldContext_->RemoveEntity(gameEntity);
            continue;
        }
        float remaining = droppedItemComponent->GetRemainingTime();
        remaining -= delta;
        droppedItemComponent->SetRemainingTime(remaining);

        float cooldown = droppedItemComponent->GetPickupCooldown();
        if (cooldown > 0.0F) {
            cooldown -= delta;
            droppedItemComponent->SetPickupCooldown(cooldown);
        }
    }
}

void glimmer::DroppedItemSystem::Render(SDL_Renderer *renderer) {
    auto entity = worldContext_->GetEntityIDWithComponents<DroppedItemComponent, Transform2DComponent>();
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraTransform2D = worldContext_->GetCameraTransform2D();
    float size = DROPPED_ITEM_SIZE * cameraComponent->GetZoom();

    for (auto gameEntity: entity) {
        auto *droppedItemComponent = worldContext_->GetComponent<DroppedItemComponent>(
            gameEntity);
        auto *transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(gameEntity);
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr) {
            continue;
        }
        Item *item = droppedItemComponent->GetItem();
        if (item == nullptr) {
            continue;
        }
        auto icon = item->GetIcon();
        if (icon == nullptr) {
            continue;
        }
        if (cameraComponent->IsPointInViewport(cameraTransform2D->GetPosition(), transform2DComponent->GetPosition())) {
            const auto worldPos = transform2DComponent->GetPosition();
            const CameraVector2D viewport = cameraComponent->WorldToScreen(
                cameraTransform2D->GetPosition(), worldPos);
            SDL_FRect dstRect{
                viewport.x - size * 0.5F,
                viewport.y - size * 0.5F,
                size,
                size
            };

            SDL_RenderTexture(renderer, icon, nullptr, &dstRect);
        }
    }
}

std::string glimmer::DroppedItemSystem::GetName() {
    return "glimmer.DroppedItemSystem";
}
