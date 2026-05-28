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

            if (TileLayerComponent::TileToWorldCenter(point).Distance(playerPosition) / TILE_SIZE >
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

glimmer::BlueprintSystem::BlueprintSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    RequireComponent<TileLayerComponent>();
    appContext_ = worldContext->GetAppContext();
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    if (gameEntities.empty())
    {
        return;
    }
    tileLayerComponent_ = worldContext_->GetComponent<TileLayerComponent>(gameEntities[0]);
    cameraComponent_ = worldContext_->GetCameraComponent();
    cameraTransform2DComponent_ = worldContext->GetCameraTransform2D();
    preloadColors_ = appContext_->GetPreloadColors();
    const GameEntity::ID playerEntity = worldContext_->GetPlayerEntity();
    playerComponent_ = worldContext_->GetComponent<PlayerComponent>(playerEntity);
    playerTransform2DComponent_ = worldContext_->GetComponent<Transform2DComponent>(playerEntity);
    blueprintComponent_ = worldContext_->GetBlueprintComponent();
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
    if (playerComponent_ == nullptr)
    {
        return;
    }
    if (playerTransform2DComponent_ == nullptr)
    {
        return;
    }
    if (tileLayerComponent_ == nullptr)
    {
        return;
    }
    heldTile_ = nullptr;
    blockRects_.clear();
    std::vector<GameEntity::ID> entities = worldContext_->GetEntityIDWithComponents<TilePlacementForbiddenZoneComponent,
        Transform2DComponent>();
    if (!entities.empty())
    {
        for (uint32_t entity : entities)
        {
            auto transform2DComponent = worldContext_->GetComponent<Transform2DComponent>(entity);
            if (transform2DComponent == nullptr)
            {
                continue;
            }
            auto tilePlacementForbiddenZone = worldContext_->GetComponent<TilePlacementForbiddenZoneComponent>(entity);
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
    Item* item = playerComponent_->item;
    TileVector2D leftBottom = {0, 0};
    WorldVector2D playerPosition = playerTransform2DComponent_->GetPosition();
    if (item != nullptr)
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
    const float zoom = cameraComponent_->GetZoom();
    //The coordinates used in the code logic need to be -1.
    //代码逻辑用的坐标需要-1。
    blueprintComponent_->SetTopLeftVector({leftBottom.x, leftBottom.y + tileHeight - 1});
    //Drawing the coordinates doesn't require -1.
    //绘制坐标不不需要-1。
    const CameraVector2D topLeftCamera = cameraComponent_->GetViewPortPosition(
        cameraTransform2DComponent_->GetPosition(),
        TileLayerComponent::TileToWorld({leftBottom.x, leftBottom.y + tileHeight}) -
        WorldVector2D(HALF_TILE_SIZE * zoom, HALF_TILE_SIZE * zoom)
    );
    const CameraVector2D focusWorldTileCamera = cameraComponent_->GetViewPortPosition(
        cameraTransform2DComponent_->GetPosition(),
        TileLayerComponent::TileToWorld({focusPosition.x, focusPosition.y + 1}) - WorldVector2D(
            HALF_TILE_SIZE * zoom, HALF_TILE_SIZE * zoom));
    SDL_FRect renderQuad;
    renderQuad.w = static_cast<float>(tileWidth) * TILE_SIZE * zoom;
    renderQuad.h = static_cast<float>(tileHeight) * TILE_SIZE * zoom;
    if (heldTile_ == nullptr)
    {
        renderQuad.x = focusWorldTileCamera.x;
        renderQuad.y = focusWorldTileCamera.y;
    }
    else
    {
        renderQuad.x = topLeftCamera.x;
        renderQuad.y = topLeftCamera.y;
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
                    const CameraVector2D tileScreenPos = cameraComponent_->GetViewPortPosition(
                        cameraTransform2DComponent_->GetPosition(), TileLayerComponent::TileToWorld(currTile));
                    SDL_FRect indicatorRenderQuad;
                    indicatorRenderQuad.w = TILE_SIZE * zoom;
                    indicatorRenderQuad.h = TILE_SIZE * zoom;
                    indicatorRenderQuad.x = tileScreenPos.x - HALF_TILE_SIZE;
                    indicatorRenderQuad.y = tileScreenPos.y - HALF_TILE_SIZE;
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

std::string glimmer::BlueprintSystem::GetName()
{
    return "BlueprintSystem";
}
