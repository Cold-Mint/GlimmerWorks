//
// Created by Cold-Mint on 2025/11/2.
//

#include "TileLayerSystem.h"

#include "../../Constants.h"
#include "../component/CameraComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/Transform2DComponent.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "core/utils/ColorUtils.h"
#include "core/world/Tile.h"
#include "core/world/generator/TileLayerType.h"

glimmer::TileLayerSystem::TileLayerSystem(WorldContext* worldContext)
    : GameSystem(worldContext)
{
    RequireComponent<TileLayerComponent>();
}

void glimmer::TileLayerSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_ == nullptr)
    {
        return;
    }
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
    const auto* cameraComponent = worldContext_->GetCameraComponent();
    const auto* cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr)
    {
        return;
    }
    if (cameraPos == nullptr)
    {
        return;
    }
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    if (gameEntities.empty())
    {
        return;
    }
    auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(gameEntities[0]);
    if (tileLayerComponent == nullptr)
    {
        return;
    }
    auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
    const float zoom = cameraComponent->GetZoom();
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
            Vector2DI offset;
            offset.ReadVector2DIMessage(tileState->offset());
            const TileVector2D tileTopLeftPosition = tileCoord + offset;
            uint64_t tileTopLeftFingerprint = TileLayerComponent::GenerateTileFingerprint(
                tileTopLeftPosition, tile->GetLayerType());
            if (drawnTiles.contains(tileTopLeftFingerprint))
            {
                continue;
            }
            drawnTiles.emplace(tileTopLeftFingerprint);
            const CameraVector2D tileTopLeftCamera = cameraComponent->GetViewPortPosition(
                cameraPos->GetPosition(), TileLayerComponent::TileToWorld(TileVector2D{
                    tileTopLeftPosition.x, tileTopLeftPosition.y + 1
                }) - WorldVector2D{HALF_TILE_SIZE * zoom, HALF_TILE_SIZE * zoom});
            SDL_FRect renderQuad;
            renderQuad.w = static_cast<float>(tileState->width()) * TILE_SIZE * zoom;
            renderQuad.h = static_cast<float>(tileState->height()) * TILE_SIZE * zoom;
            renderQuad.x = tileTopLeftCamera.x;
            renderQuad.y = tileTopLeftCamera.y;
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
    const auto* camera = worldContext_->GetCameraComponent();
    if (camera == nullptr)
    {
        return false;
    }
    const auto* cameraTransform = worldContext_->GetCameraTransform2D();
    if (cameraTransform == nullptr)
    {
        return false;
    }
    const WorldVector2D worldPos = camera->GetWorldPosition(
        cameraTransform->GetPosition(),
        CameraVector2D(event.motion.x, event.motion.y)
    );
    auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    for (auto& entity : tileLayerEntities)
    {
        auto* layer = worldContext_->GetComponent<TileLayerComponent>(entity);
        if (layer == nullptr)
        {
            continue;
        }
        layer->SetFocusPosition(TileLayerComponent::WorldToTile(worldPos));
    }
    return true;
}

std::string glimmer::TileLayerSystem::GetName()
{
    return "glimmer.TileLayerSystem";
}
