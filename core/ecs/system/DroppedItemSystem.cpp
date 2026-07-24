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

#include "core/world/WorldContext.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/ecs/component/DroppedItemComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/CoordinateTransformer.h"


void glimmer::DroppedItemSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityShortCut* entityShortCut = GetEntityShortCut();
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        transform2dCount = count;
        if (cameraTransform2DComponent_ == nullptr)
        {
            cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
        }
    }
    if (gameComponentType == COMPONENT_DROPPED_ITEM)
    {
        droppedItemCount = count;
    }
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (transform2dCount > 0 && droppedItemCount > 0)
    {
        droppedEntities_ = entityManager->GetEntityIDWithComponents({COMPONENT_TRANSFORM_2D, COMPONENT_DROPPED_ITEM});
    }
}

uint8_t glimmer::DroppedItemSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_DROPPED_ITEM;
}

glimmer::DroppedItemSystem::DroppedItemSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_DROPPED_ITEM);
    WatchComponent(COMPONENT_CAMERA);
    Init();
}

void glimmer::DroppedItemSystem::Update(float delta)
{
    EntityManager* entityManager = GetEntityManager();
    for (const auto& gameEntity : droppedEntities_)
    {
        auto droppedItemComponent = entityManager->GetComponent<DroppedItemComponent>(
            gameEntity);
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(gameEntity);
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr)
        {
            continue;
        }
        if (droppedItemComponent->IsExpired())
        {
            entityManager->RemoveEntity(gameEntity);
            continue;
        }
        float remaining = droppedItemComponent->GetRemainingTime();
        remaining -= delta;
        droppedItemComponent->SetRemainingTime(remaining);

        float cooldown = droppedItemComponent->GetPickupCooldown();
        if (cooldown > 0.0F)
        {
            cooldown -= delta;
            droppedItemComponent->SetPickupCooldown(cooldown);
        }
    }
}

void glimmer::DroppedItemSystem::Render(SDL_Renderer* renderer)
{
    EntityManager* entityManager = GetEntityManager();
    if (cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    float size = DROPPED_ITEM_SIZE * cameraComponent_->GetZoom();

    for (auto gameEntity : droppedEntities_)
    {
        auto droppedItemComponent = entityManager->GetComponent<DroppedItemComponent>(
            gameEntity);
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(gameEntity);
        if (droppedItemComponent == nullptr || transform2DComponent == nullptr)
        {
            continue;
        }
        Item* item = droppedItemComponent->GetItem();
        if (item == nullptr)
        {
            continue;
        }
        auto icon = item->GetIcon();
        if (icon == nullptr)
        {
            continue;
        }
        if (cameraComponent_->IsPointInViewport(cameraTransform2DComponent_->GetPosition(),
                                                transform2DComponent->GetPosition()))
        {
            const auto worldPos = transform2DComponent->GetPosition();
            const ScreenVector2D viewport = CoordinateTransformer::WorldToScreen(
                cameraTransform2DComponent_->GetPosition(), worldPos, cameraComponent_->GetSize(),
                cameraComponent_->GetZoom());
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

glimmer::GameSystemType glimmer::DroppedItemSystem::GetGameSystemType() const
{
    return GameSystemType::DroppedItemSystem;
}
