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
#include "DraggableSystem.h"

#include "core/Constants.h"
#include "core/math/CoordinateTransformer.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/DraggableComponent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"


void glimmer::DraggableSystem::OnActivationChanged(bool activeStatus)
{
    if (!activeStatus)
    {
        DropItem();
    }
}


SDL_FRect glimmer::DraggableSystem::DraggableBorder(const ItemSlotComponent* itemSlotComponent,
                                                    const float uiScale)
{
    SDL_FRect border = {0, 0, 0, 0};
    if (itemSlotComponent == nullptr)
    {
        return border;
    }
    const ScreenVector2D size = CoordinateTransformer::DesignToScreen(itemSlotComponent->GetSize(), uiScale);
    const ScreenVector2D startCoordinates = CoordinateTransformer::DesignToScreen(
        itemSlotComponent->GetPosition(), uiScale);
    border.x = startCoordinates.x;
    border.y = startCoordinates.y;
    border.w = size.x;
    border.h = size.y;
    return border;
}

bool glimmer::DraggableSystem::DropItem()
{
    const auto entityManager = GetEntityManager();
    const auto worldContext = GetWorldContext();
    if (item_ == nullptr)
    {
        return false;
    }
    if (cameraTransform2D_ == nullptr)
    {
        return false;
    }
    uint32_t droppedEntity = entityManager->AddEntity();
    DroppedItemCreator droppedItemCreator{worldContext};
    droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                              DroppedItemCreator::GetResourceRef());
    droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                              DroppedItemCreator::GetEntityItemMessage(
                                                  cameraTransform2D_
                                                  ->
                                                  GetPosition(),
                                                  std::move(item_),
                                                  2));
    return true;
}

void glimmer::DraggableSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    EntityManager* entityManager = GetEntityManager();
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        if (cameraTransform2D_ == nullptr)
        {
            cameraTransform2D_ = entityShortCut->GetCameraTransform2DComponent();
        }
#if  !defined(NDEBUG)
        transform2DCount_ = count;
#endif
    }
    if (gameComponentType == COMPONENT_DRAGGABLE)
    {
        draggableCount_ = count;
    }
    if (gameComponentType == COMPONENT_ITEM_SLOT)
    {
        itemSlotCount_ = count;
    }
#if  defined(NDEBUG)
    if (draggableCount_ > 0 && itemSlotCount_ > 0)
    {
        std::vector<GameEntityID> entities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_DRAGGABLE, COMPONENT_ITEM_SLOT
        });
        for (GameEntityID entity : entities_)
        {
            ItemSlotComponent* itemSlotComponent = entityManager->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            itemSlotComponentVector_.emplace_back(itemSlotComponent);
        }
    }
#else
    if (draggableCount_ > 0 && itemSlotCount_ > 0 && transform2DCount_ > 0)
    {
        std::vector<GameEntityID> entities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_DRAGGABLE, COMPONENT_ITEM_SLOT, COMPONENT_TRANSFORM_2D
        });
        for (GameEntityID entity : entities_)
        {
            ItemSlotComponent* itemSlotComponent = entityManager->GetComponent<ItemSlotComponent>(entity);
            if (itemSlotComponent == nullptr)
            {
                continue;
            }
            itemSlotComponentVector_.emplace_back(itemSlotComponent);
        }
    }
#endif
}

glimmer::DraggableSystem::DraggableSystem(WorldContext* worldContext)
    : GameSystem(worldContext),
      appContext_(worldContext->GetAppContext())
{
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_DRAGGABLE);
    WatchComponent(COMPONENT_ITEM_SLOT);
    Init();
}

void glimmer::DraggableSystem::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}


uint8_t glimmer::DraggableSystem::GetRenderOrder()
{
    return RENDER_ORDER_DRAGGABLE;
}

void glimmer::DraggableSystem::Render(SDL_Renderer* renderer)
{
    const WorldContext* worldContext = GetWorldContext();
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    if (item_ != nullptr)
    {
        float mouseX = 0;
        float mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        const SDL_FRect dst = {
            mouseX + 6, mouseY + 24, ITEM_SLOT_SIZE * uiScale_,
            ITEM_SLOT_SIZE * uiScale_
        };
        SDL_Texture* texture = item_->GetIcon();
        if (texture != nullptr)
        {
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
        }
    }

#if  !defined(NDEBUG)
    if (appContext->GetConfig()->debug.displayDraggableTarget)
    {
        if (cameraComponent_ == nullptr)
        {
            return;
        }
        if (cameraTransform2D_ == nullptr)
        {
            return;
        }
        if (itemSlotComponentVector_.empty())
        {
            return;
        }

        Color draggableColor = appContext->GetGraphicsContext()->GetPreloadColors()->debugColor.draggableColor;
        SDL_SetRenderDrawColor(renderer, draggableColor.r, draggableColor.g, draggableColor.b, draggableColor.a);
        for (auto itemSlotComponent : itemSlotComponentVector_)
        {
            SDL_FRect border = DraggableBorder(itemSlotComponent, uiScale_);
            if (border.h <= 0 || border.w <= 0)
            {
                continue;
            }
            SDL_RenderRect(renderer, &border);
        }
    }
#endif
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::DraggableSystem::HandleLeftMouseButtonDown()
{
    if (cameraComponent_ == nullptr)
    {
        return false;
    }
    if (cameraTransform2D_ == nullptr)
    {
        return false;
    }
    if (itemSlotComponentVector_.empty())
    {
        return false;
    }
    bool changed = false;
    for (auto itemSlotComponent : itemSlotComponentVector_)
    {
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        const SDL_FRect border = DraggableBorder(itemSlotComponent, uiScale_);
        if (border.h <= 0 || border.w <= 0)
        {
            continue;
        }
        float mouseX = 0;
        float mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= border.x && mouseX <= border.x + border.w &&
            mouseY >= border.y && mouseY <= border.y + border.h)
        {
            item_ = itemSlotComponent->ReplaceItem(std::move(item_));
            changed = true;
            break;
        }
    }
    if (!changed)
    {
        return DropItem();
    }
    return false;
}

bool glimmer::DraggableSystem::HandleEvent(const SDL_Event& event)
{
    const WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        return HandleLeftMouseButtonDown();
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        return DropItem();
    }
    return false;
}

glimmer::GameSystemType glimmer::DraggableSystem::GetGameSystemType() const
{
    return GameSystemType::DraggableSystem;
}
