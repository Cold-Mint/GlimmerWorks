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
#include "TileLayerSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/TileLayerComponent.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "core/utils/ColorUtils.h"
#include "core/world/Tile.h"
#include "core/world/generator/TileLayerType.h"

void glimmer::TileLayerSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_TILE_LAYER)
    {
        tileLayerComponents_.clear();
        auto tileLayerEntities = entityManager_->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        std::sort(tileLayerEntities.begin(), tileLayerEntities.end());
        for (auto& entity : tileLayerEntities)
        {
            auto* tileLayerComponent = entityManager_->GetComponent<TileLayerComponent>(entity);
            if (tileLayerComponent == nullptr)
            {
                continue;
            }
            tileLayerComponents_.emplace_back(tileLayerComponent);
        }
    }
}

glimmer::TileLayerSystem::TileLayerSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_TILE_LAYER);
}

void glimmer::TileLayerSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    if (cameraComponent_ == nullptr)
    {
        return;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return;
    }
    if (tileLayerComponents_.empty())
    {
        return;
    }
    TileLayerComponent* tileLayerComponent = tileLayerComponents_.front();
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return;
    }
    auto viewportRect = cameraComponent_->GetViewportRect(cameraTransform2DComponent_->GetPosition());
    const float zoom = cameraComponent_->GetZoom();
    std::vector<std::pair<TileVector2D, std::unique_ptr<std::vector<TileSnapshot>>>> visibleTiles =
        tileLayerComponent->GetTopVisibleTileSnapshotsInViewport(Ground | BackGround, viewportRect);
    std::unordered_set<uint64_t> drawnTiles = {};
    for (auto& [tileCoord, tileList] : visibleTiles)
    {
        const Chunk* chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tileCoord));
        Uint8 alpha = 255;
        if (chunk != nullptr)
        {
            alpha = static_cast<Uint8>(chunk->GetChunkFadeAlpha() * 255.0F);
        }
        for (const auto& tileSnapshot : *tileList)
        {
            const Tile* tile = tileSnapshot.GetTile();
            if (tile == nullptr)
            {
                continue;
            }
            const TileStateMessage* tileState = tileSnapshot.GetTileState();
            if (tileState == nullptr)
            {
                continue;
            }
            TileVector2D offset;
            offset.ReadVector2DIMessage(tileState->offset());
            const TileVector2D tileTopLeftPosition = tileCoord + offset;
            uint64_t tileTopLeftFingerprint = TileLayerComponent::GenerateTileFingerprint(
                tileTopLeftPosition, tile->GetLayerType());
            if (drawnTiles.contains(tileTopLeftFingerprint))
            {
                continue;
            }
            drawnTiles.emplace(tileTopLeftFingerprint);
            const CameraVector2D tileTopLeftCamera = cameraComponent_->WorldToScreen(
                cameraTransform2DComponent_->GetPosition(), TileLayerComponent::TileToWorld(TileVector2D{
                    tileTopLeftPosition.x, tileTopLeftPosition.y
                }));
            float width = static_cast<float>(tileState->width()) * TILE_SIZE * zoom;
            float height = static_cast<float>(tileState->height()) * TILE_SIZE * zoom;
            SDL_FRect renderQuad;
            renderQuad.w = width;
            renderQuad.h = height;
            renderQuad.x = tileTopLeftCamera.x - renderQuad.w * 0.5F;
            renderQuad.y = tileTopLeftCamera.y - renderQuad.h * 0.5F;
            const Color* finalLightColor = worldContext_->GetLightingBuffer()->GetFinalLightColor(tileCoord);
#if  defined(NDEBUG)
            if (finalLightColor == nullptr)
            {
                continue;
            }
            if (finalLightColor->a == 0)
            {
                //Do not draw tiles that do not emit light at all.
                //不绘制完全不发光的瓦片。
                continue;
            }
#else
            if (config->light.enable)
            {
                if (finalLightColor == nullptr)
                {
                    continue;
                }
                if (finalLightColor->a == 0)
                {
                    //Do not draw tiles that do not emit light at all.
                    //不绘制完全不发光的瓦片。
                    continue;
                }
            }
#endif
            auto texture = tile->GetTexture();
            if (texture != nullptr)
            {
                SDL_SetTextureAlphaMod(texture, alpha);
                if (!SDL_RenderTexture(renderer, texture, nullptr, &renderQuad))
                {
                    LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
                }
                SDL_SetTextureAlphaMod(texture, 255);
            }
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::TileLayerSystem::GetRenderOrder()
{
    return RENDER_ORDER_TILE_LAYER;
}

bool glimmer::TileLayerSystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (event.type != SDL_EVENT_MOUSE_MOTION)
    {
        return false;
    }
    if (cameraComponent_ == nullptr)
    {
        return false;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return false;
    }
    const WorldVector2D worldPos = cameraComponent_->ScreenToWorld(
        cameraTransform2DComponent_->GetPosition(),
        CameraVector2D(event.motion.x, event.motion.y)
    );
    for (auto tileLayerComponent : tileLayerComponents_)
    {
        tileLayerComponent->SetFocusPosition(TileLayerComponent::WorldToTile(worldPos));
    }
    return true;
}

glimmer::GameSystemType glimmer::TileLayerSystem::GetGameSystemType() const
{
    return GameSystemType::TileLayerSystem;
}
