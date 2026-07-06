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
#include "core/math/CoordinateTransformer.h"
#include "core/world/WorldContext.h"

std::vector<bool> glimmer::BlueprintSystem::CheckRectPlacementValidity(const Tile* tile, const TileVector2D& leftBottom,
                                                                       const WorldVector2D& playerPosition,
                                                                       const uint8_t tileWidth,
                                                                       const uint8_t tileHeight) const
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

            if ((CoordinateTransformer::TileToWorld(point) + WorldVector2D{HALF_TILE_SIZE, HALF_TILE_SIZE}).
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
    EntityManager* entityManager = GetEntityManager();
    const EntityShortCut* entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_TILE_LAYER)
    {
        auto tileLayerEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        if (!tileLayerEntities.empty())
        {
            tileLayerComponent_ = entityManager->GetComponent<TileLayerComponent>(tileLayerEntities[0]);
        }
    }
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D)
    {
        if (cameraTransform2DComponent_ == nullptr)
        {
            cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
        }
        transform2DCount_ = count;
    }
    if (gameComponentType == COMPONENT_BLUEPRINT && blueprintComponent_ == nullptr)
    {
        blueprintComponent_ = entityShortCut->GetBlueprintComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && WorldContext::IsEmptyEntityId(player))
    {
        player = entityShortCut->GetPlayer();
    }
    if (gameComponentType == COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE)
    {
        tilePlacementForbiddenZoneCount_ = count;
    }
    if (transform2DCount_ > 0 && tilePlacementForbiddenZoneCount_ > 0)
    {
        entities_ = entityManager->GetEntityIDWithComponents({
            COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE, COMPONENT_TRANSFORM_2D
        });
    }
}

glimmer::BlueprintSystem::BlueprintSystem(WorldContext* worldContext)
    : GameSystem(worldContext),
      preloadColors_(worldContext->GetAppContext()->GetPreloadColors())
{
    WatchComponent(COMPONENT_TILE_LAYER);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_PLAYER);
    WatchComponent(COMPONENT_BLUEPRINT);
    WatchComponent(COMPONENT_TILE_PLACEMENT_FORBIDDEN_ZONE);
    Init();
}

uint8_t glimmer::BlueprintSystem::GetRenderOrder()
{
    return RENDER_ORDER_BLUEPRINT;
}

void glimmer::BlueprintSystem::UpdateBlockRects()
{
    EntityManager* entityManager = GetEntityManager();
    blockRects_.clear();
    if (entities_.empty())
    {
        return;
    }
    for (uint32_t entity : entities_)
    {
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(entity);
        if (transform2DComponent == nullptr)
        {
            continue;
        }
        auto tilePlacementForbiddenZone = entityManager->GetComponent<TilePlacementForbiddenZoneComponent>(entity);
        if (tilePlacementForbiddenZone == nullptr)
        {
            continue;
        }
        const TileVector2D originTileVector2D =
            CoordinateTransformer::WorldToTile(transform2DComponent->GetPosition());
        const int startX = originTileVector2D.x + tilePlacementForbiddenZone->GetOffsetX();
        const int startY = originTileVector2D.y + tilePlacementForbiddenZone->GetOffsetY();
        blockRects_.push_back({
            startX, startY, tilePlacementForbiddenZone->GetWidth(), tilePlacementForbiddenZone->GetHeight()
        });
    }
}

glimmer::TileVector2D glimmer::BlueprintSystem::SetupHeldTileInfo(const TileVector2D& focusPosition,
                                                                  const WorldVector2D& focusWorldTilePos,
                                                                  const WorldVector2D& playerPosition,
                                                                  uint8_t& tileWidth,
                                                                  uint8_t& tileHeight,
                                                                  TileVector2D& tileAnchor)
{
    EntityManager* entityManager = GetEntityManager();
    TileVector2D leftBottom = {0, 0};
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(player);
    if (playerComponent == nullptr)
    {
        return leftBottom;
    }
    Item* item = playerComponent->GetItem();
    if (item == nullptr || item->GetAmount() <= 0)
    {
        return leftBottom;
    }
    auto tileItem = dynamic_cast<TileItem*>(item);
    if (tileItem == nullptr)
    {
        return leftBottom;
    }
    heldTile_ = tileItem->GetTile();
    if (heldTile_ == nullptr)
    {
        return leftBottom;
    }
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
    return leftBottom;
}

SDL_FRect glimmer::BlueprintSystem::CalculateRenderQuad(const TileVector2D& focusPosition,
                                                        const TileVector2D& topLeftVector,
                                                        uint8_t tileWidth,
                                                        uint8_t tileHeight)
{
    SDL_FRect renderQuad;
    const float zoom = cameraComponent_->GetZoom();
    renderQuad.w = static_cast<float>(tileWidth) * TILE_SIZE * zoom;
    renderQuad.h = static_cast<float>(tileHeight) * TILE_SIZE * zoom;
    if (heldTile_ == nullptr)
    {
        const ScreenVector2D focusWorldTileCamera = CoordinateTransformer::WorldToScreen(
            cameraTransform2DComponent_->GetPosition(),
            CoordinateTransformer::TileToWorld({focusPosition.x, focusPosition.y}),
            cameraComponent_->GetSize(), cameraComponent_->GetZoom());
        renderQuad.x = focusWorldTileCamera.x - renderQuad.w * 0.5F;
        renderQuad.y = focusWorldTileCamera.y - renderQuad.h * 0.5F;
        return renderQuad;
    }
    const ScreenVector2D topLeftCamera = CoordinateTransformer::WorldToScreen(
        cameraTransform2DComponent_->GetPosition(),
        CoordinateTransformer::TileToWorld(topLeftVector), cameraComponent_->GetSize(),
        cameraComponent_->GetZoom()
    );
    renderQuad.x = topLeftCamera.x - renderQuad.w * 0.5F;
    renderQuad.y = topLeftCamera.y - renderQuad.w * 0.5F;
    return renderQuad;
}

void glimmer::BlueprintSystem::RenderBlueprintTexture(SDL_Renderer* renderer, const SDL_FRect& renderQuad)
{
    if (heldTile_ == nullptr || !heldTile_->EnableBlueprint())
    {
        return;
    }
    SDL_Texture* blueprintTexture = heldTile_->GetBlueprintTexture();
    if (blueprintTexture == nullptr)
    {
        return;
    }
    Uint8 originalAlpha = 255;
    SDL_GetTextureAlphaMod(blueprintTexture, &originalAlpha);
    SDL_SetTextureAlphaMod(blueprintTexture, 128);
    SDL_RenderTexture(renderer, blueprintTexture, nullptr, &renderQuad);
    SDL_SetTextureAlphaMod(blueprintTexture, originalAlpha);
}

void glimmer::BlueprintSystem::RenderBlueprintMask(SDL_Renderer* renderer, const std::vector<bool>& checkRectResult,
                                                   const TileVector2D& leftBottom, uint8_t tileWidth,
                                                   uint8_t tileHeight)
{
    if (heldTile_ == nullptr || !heldTile_->EnableBlueprintMask())
    {
        return;
    }
    const float zoom = cameraComponent_->GetZoom();
    for (int i = 0; i < static_cast<int>(checkRectResult.size()) - 1; i++)
    {
        int relX = i % tileWidth;
        int relY = i / tileWidth;
        TileVector2D currTile = {leftBottom.x + relX, leftBottom.y + relY};
        const ScreenVector2D tileScreenPos = CoordinateTransformer::WorldToScreen(
            cameraTransform2DComponent_->GetPosition(), CoordinateTransformer::TileToWorld(currTile),
            cameraComponent_->GetSize(), cameraComponent_->GetZoom());
        SDL_FRect indicatorRenderQuad;
        indicatorRenderQuad.w = TILE_SIZE * zoom;
        indicatorRenderQuad.h = TILE_SIZE * zoom;
        indicatorRenderQuad.x = tileScreenPos.x - indicatorRenderQuad.w * 0.5f;
        indicatorRenderQuad.y = tileScreenPos.y - indicatorRenderQuad.h * 0.5f;
        if (checkRectResult[i])
        {
            if (!heldTile_->DrawValidBlueprintColor())
            {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, preloadColors_->blueprint.validColor.r,
                                   preloadColors_->blueprint.validColor.g,
                                   preloadColors_->blueprint.validColor.b,
                                   preloadColors_->blueprint.validColor.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, preloadColors_->blueprint.invalidColor.r,
                                   preloadColors_->blueprint.invalidColor.g,
                                   preloadColors_->blueprint.invalidColor.b,
                                   preloadColors_->blueprint.invalidColor.a);
        }
        SDL_RenderFillRect(renderer, &indicatorRenderQuad);
    }
}

void glimmer::BlueprintSystem::Render(SDL_Renderer* renderer)
{
    const WorldContext* worldContext = GetWorldContext();
    EntityManager* entityManager = GetEntityManager();
    if (worldContext == nullptr)
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
    UpdateBlockRects();

    uint8_t tileWidth = 1;
    uint8_t tileHeight = 1;
    TileVector2D tileAnchor = {0, 0};
    const TileVector2D& focusPosition = tileLayerComponent_->GetFocusPosition();
    const WorldVector2D focusWorldTilePos = CoordinateTransformer::TileToWorld(focusPosition);
    auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(player);
    WorldVector2D playerPosition = transform2DComponent->GetPosition();
    TileVector2D leftBottom = SetupHeldTileInfo(focusPosition, focusWorldTilePos, playerPosition,
                                                tileWidth, tileHeight, tileAnchor);
    auto topLeftVector = TileVector2D{leftBottom.x, leftBottom.y + tileHeight - 1};
    blueprintComponent_->SetTopLeftVector(topLeftVector);
    SDL_FRect renderQuad = CalculateRenderQuad(focusPosition, topLeftVector, tileWidth, tileHeight);

    SDL_SetRenderDrawColor(renderer, preloadColors_->game.focusTileBorderColor.r,
                           preloadColors_->game.focusTileBorderColor.g,
                           preloadColors_->game.focusTileBorderColor.b,
                           preloadColors_->game.focusTileBorderColor.a);
    SDL_RenderRect(renderer, &renderQuad);
    if (heldTile_ == nullptr)
    {
        blueprintComponent_->SetCanPlace(false);
        AppContext::RestoreColorRenderer(renderer);
        return;
    }
    RenderBlueprintTexture(renderer, renderQuad);
    std::vector<bool> checkRectResult = CheckRectPlacementValidity(heldTile_, leftBottom, playerPosition, tileWidth,
                                                                   tileHeight);
    if (checkRectResult.empty())
    {
        blueprintComponent_->SetCanPlace(false);
        AppContext::RestoreColorRenderer(renderer);
        return;
    }
    RenderBlueprintMask(renderer, checkRectResult, leftBottom, tileWidth, tileHeight);
    blueprintComponent_->SetCanPlace(checkRectResult[checkRectResult.size() - 1]);
    AppContext::RestoreColorRenderer(renderer);
}

glimmer::GameSystemType glimmer::BlueprintSystem::GetGameSystemType() const
{
    return GameSystemType::BlueprintSystem;
}
