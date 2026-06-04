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
#include "BlueprintSystem.h"

#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/TilePlacementForbiddenZoneComponent.h"
#include "core/inventory/TileItem.h"
#include "core/world/WorldContext.h"

std::vector<bool> glimmer::BlueprintSystem::CheckRectPlacementValidity(const Tile* tile, TileVector2D leftBottom,
                                                                       WorldVector2D playerPosition, uint8_t tileWidth,
                                                                       uint8_t tileHeight) const
{
    std::vector<bool> result;
    result.resize(tileHeight * tileWidth + 1, false);
    bool sum = true;
    const TileLayerType layerType = tile->GetLayerType();
    for (int x = 0; x < tileWidth; x++)
    {
        for (int y = 0; y < tileHeight; y++)
        {
            const int index = y * tileWidth + x;
            TileVector2D point = {x + leftBottom.x, y + leftBottom.y};
            if (y > WORLD_MAX_Y || y < WORLD_MIN_Y ||
                x > WORLD_MAX_X ||
                x < WORLD_MIN_X)
            {
                result[index] = false;
                sum = false;
                continue;
            }
            if (layerType == Ground)
            {
                bool blocked = false;
                for (auto blockRect : blockRects_)
                {
                    // If the coordinate point falls within the rectangular range, it is determined as being in shadow.
                    // 坐标点落在矩形范围内则判定遮挡
                    if (point.x >= blockRect.x && point.x < blockRect.x + blockRect.w
                        && point.y >= blockRect.y && point.y < blockRect.y + blockRect.h)
                    {
                        blocked = true;
                        break;
                    }
                }
                if (blocked)
                {
                    result[index] = false;
                    sum = false;
                    continue;
                }
            }
            const Tile* currentTile = tileLayerComponent_->GetTile(layerType, point);
            if (currentTile == nullptr)
            {
                result[index] = false;
                sum = false;
                continue;
            }

            if ((TileLayerComponent::TileToWorld(point) + WorldVector2D{HALF_TILE_SIZE, HALF_TILE_SIZE}).
                Distance(playerPosition) / TILE_SIZE >
                TILE_PLACE_RANGE)
            {
                result[index] = false;
                sum = false;
                continue;
            }
            if (!currentTile->IsOverwritable())
            {
                result[index] = false;
                sum = false;
                continue;
            }
            result[index] = true;
        }
    }

    result[result.size() - 1] = sum;
    return result;
}

void glimmer::BlueprintSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_TILE_LAYER)
    {
        tileLayerEntities_ = entityManager_->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        if (!tileLayerEntities_.empty())
        {
            tileLayerComponent_ = entityManager_->GetComponent<TileLayerComponent>(tileLayerEntities_[0]);
        }
    }
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        if (cameraTransform2DComponent_ == nullptr)
        {
            cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
        }
        transform2DCount_ = count;
    }
    if (gameComponentType == COMPONENT_BLUEPRINT && blueprintComponent_ == nullptr)
    {
        blueprintComponent_ = entityShortCut_->GetBlueprintComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && WorldContext::IsEmptyEntityId(player))
    {
        player = entityShortCut_->GetPlayer();
    }
    if (gameComponentType == COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE)
    {
        tilePlacementForbiddenZoneCount_ = count;
    }
    if (transform2DCount_ > 0 && tilePlacementForbiddenZoneCount_ > 0)
    {
        entities_ = entityManager_->GetEntityIDWithComponents({
            COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE, COMPONENT_TRANSFORM_2D
        });
    }
}

glimmer::BlueprintSystem::BlueprintSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_TILE_LAYER);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_PLAYER);
    WatchComponent(COMPONENT_BLUEPRINT);
    WatchComponent(COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE);

}

uint8_t glimmer::BlueprintSystem::GetRenderOrder()
{
    return RENDER_ORDER_BLUEPRINT;
}

void glimmer::BlueprintSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (blueprintComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    if (tileLayerComponent_ == nullptr)
    {
        return;
    }
    if (WorldContext::IsEmptyEntityId(player))
    {
        return;
    }
    heldTile_ = nullptr;
    blockRects_.clear();
    if (!entities_.empty())
    {
        for (uint32_t entity : entities_)
        {
            auto transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(entity);
            if (transform2DComponent == nullptr)
            {
                continue;
            }
            auto tilePlacementForbiddenZone = entityManager_->GetComponent<TilePlacementForbiddenZoneComponent>(entity);
            if (tilePlacementForbiddenZone == nullptr)
            {
                continue;
            }
            const TileVector2D originTileVector2D =
                TileLayerComponent::WorldToTile(transform2DComponent->GetPosition());
            const int startX = originTileVector2D.x + tilePlacementForbiddenZone->GetOffsetX();
            const int startY = originTileVector2D.y + tilePlacementForbiddenZone->GetOffsetY();
            blockRects_.push_back({
                startX, startY, tilePlacementForbiddenZone->GetWidth(), tilePlacementForbiddenZone->GetHeight()
            });
        }
    }
    uint8_t tileWidth = 1;
    uint8_t tileHeight = 1;
    TileVector2D tileAnchor = {0, 0};
    const TileVector2D& focusPosition = tileLayerComponent_->GetFocusPosition();
    const WorldVector2D focusWorldTilePos = TileLayerComponent::TileToWorld(focusPosition);
    PlayerComponent* playerComponent = entityManager_->GetComponent<PlayerComponent>(player);
    Transform2DComponent* transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(player);
    Item* item = playerComponent->item;
    TileVector2D leftBottom = {0, 0};
    WorldVector2D playerPosition = transform2DComponent->GetPosition();
    if (item != nullptr && item->GetAmount() > 0)
    {
        auto tileItem = dynamic_cast<TileItem*>(item);
        if (tileItem != nullptr)
        {
            heldTile_ = tileItem->GetTile();
            if (heldTile_ != nullptr)
            {
                tileWidth = heldTile_->GetTileWidth();
                tileHeight = heldTile_->GetTileHeight();
                tileAnchor = *heldTile_->GetTileAnchor();
                bool atLeft = focusWorldTilePos.x < playerPosition.x;
                if (atLeft && heldTile_->IsAllowDirAdjustAnchor())
                {
                    tileAnchor.x = tileWidth - 1 - tileAnchor.x;
                }
                leftBottom.x = focusPosition.x - tileAnchor.x;
                leftBottom.y = focusPosition.y - tileAnchor.y;
            }
        }
    }
    auto TopLeftVector = TileVector2D{leftBottom.x, leftBottom.y + tileHeight - 1};
    blueprintComponent_->SetTopLeftVector(TopLeftVector);
    const CameraVector2D focusWorldTileCamera = cameraComponent_->WorldToScreen(
        cameraTransform2DComponent_->GetPosition(),
        TileLayerComponent::TileToWorld({focusPosition.x, focusPosition.y}));
    SDL_FRect renderQuad;
    const float zoom = cameraComponent_->GetZoom();
    renderQuad.w = static_cast<float>(tileWidth) * TILE_SIZE * zoom;
    renderQuad.h = static_cast<float>(tileHeight) * TILE_SIZE * zoom;
    if (heldTile_ == nullptr)
    {
        renderQuad.x = focusWorldTileCamera.x - renderQuad.w * 0.5F;
        renderQuad.y = focusWorldTileCamera.y - renderQuad.h * 0.5F;
    }
    else
    {
        const CameraVector2D topLeftCamera = cameraComponent_->WorldToScreen(
            cameraTransform2DComponent_->GetPosition(),
            TileLayerComponent::TileToWorld(TopLeftVector)
        );
        renderQuad.x = topLeftCamera.x - renderQuad.w * 0.5F;
        renderQuad.y = topLeftCamera.y - renderQuad.w * 0.5F;
    }

    SDL_SetRenderDrawColor(renderer, preloadColors_->game.focusTileBorderColor.r,
                           preloadColors_->game.focusTileBorderColor.g,
                           preloadColors_->game.focusTileBorderColor.b,
                           preloadColors_->game.focusTileBorderColor.a);
    SDL_RenderRect(renderer, &renderQuad);
    if (heldTile_ == nullptr)
    {
        blueprintComponent_->SetCanPlace(false);
    }
    else
    {
        if (heldTile_->EnableBlueprint())
        {
            SDL_Texture* blueprintTexture = heldTile_->GetBlueprintTexture();
            if (blueprintTexture != nullptr)
            {
                Uint8 originalAlpha = 255;
                SDL_GetTextureAlphaMod(blueprintTexture, &originalAlpha);
                SDL_SetTextureAlphaMod(blueprintTexture, 128);
                SDL_RenderTexture(renderer, blueprintTexture, nullptr, &renderQuad);
                SDL_SetTextureAlphaMod(blueprintTexture, originalAlpha);
            }
        }
        std::vector<bool> checkRectResult = CheckRectPlacementValidity(heldTile_, leftBottom, playerPosition, tileWidth,
                                                                       tileHeight);
        if (checkRectResult.empty())
        {
            blueprintComponent_->SetCanPlace(false);
        }
        else
        {
            if (heldTile_->EnableBlueprintMask())
            {
                for (int i = 0; i < checkRectResult.size() - 1; i++)
                {
                    int relX = i % tileWidth;
                    int relY = i / tileWidth;
                    TileVector2D currTile = {leftBottom.x + relX, leftBottom.y + relY};
                    const CameraVector2D tileScreenPos = cameraComponent_->WorldToScreen(
                        cameraTransform2DComponent_->GetPosition(), TileLayerComponent::TileToWorld(currTile));
                    SDL_FRect indicatorRenderQuad;
                    indicatorRenderQuad.w = TILE_SIZE * zoom;
                    indicatorRenderQuad.h = TILE_SIZE * zoom;
                    indicatorRenderQuad.x = tileScreenPos.x - indicatorRenderQuad.w * 0.5;
                    indicatorRenderQuad.y = tileScreenPos.y - indicatorRenderQuad.h * 0.5;
                    if (checkRectResult[i])
                    {
                        if (heldTile_->DrawValidBlueprintColor())
                        {
                            SDL_SetRenderDrawColor(renderer, preloadColors_->blueprint.validColor.r,
                                                   preloadColors_->blueprint.validColor.g,
                                                   preloadColors_->blueprint.validColor.b,
                                                   preloadColors_->blueprint.validColor.a);
                            SDL_RenderFillRect(renderer, &indicatorRenderQuad);
                        }
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, preloadColors_->blueprint.invalidColor.r,
                                               preloadColors_->blueprint.invalidColor.g,
                                               preloadColors_->blueprint.invalidColor.b,
                                               preloadColors_->blueprint.invalidColor.a);
                        SDL_RenderFillRect(renderer, &indicatorRenderQuad);
                    }
                }
            }
            blueprintComponent_->SetCanPlace(checkRectResult[checkRectResult.size() - 1]);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

glimmer::GameSystemType glimmer::BlueprintSystem::GetGameSystemType()
{
    return GameSystemType::BlueprintSystem;
}
