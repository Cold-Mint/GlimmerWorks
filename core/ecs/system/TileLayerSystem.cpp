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

glimmer::TileLayerSystem::TileLayerSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
}

void glimmer::TileLayerSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return;
    }
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraComponent == nullptr) {
        return;
    }
    if (cameraPos == nullptr) {
        return;
    }
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    if (gameEntities.empty()) {
        return;
    }
    auto tileLayerComponent = worldContext_->GetComponent<TileLayerComponent>(gameEntities[0]);
    if (tileLayerComponent == nullptr) {
        return;
    }
    auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
    const float zoom = cameraComponent->GetZoom();
    std::vector<std::pair<TileVector2D, std::vector<const Tile *> > > visibleTiles =
            tileLayerComponent->GetTopVisibleTilesInViewport(Ground | BackGround, viewportRect);
    for (const auto &[tileCoord, tileList]: visibleTiles) {
        const Chunk *chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tileCoord));
        Uint8 alpha = 255;
        if (chunk != nullptr) {
            alpha = static_cast<Uint8>(chunk->GetChunkFadeAlpha() * 255.0F);
        }

        const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileCoord);
        const CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
            cameraPos->GetPosition(), worldTilePos);
        SDL_FRect renderQuad;
        renderQuad.w = TILE_SIZE * zoom;
        renderQuad.h = TILE_SIZE * zoom;
        renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
        renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
        for (auto tile: tileList) {
            const Color *finalLightColor = worldContext_->GetLightingBuffer()->GetFinalLightColor(tileCoord);
#if  defined(NDEBUG)
            if (finalLightColor == nullptr) {
                continue;
            }
            if (finalLightColor->a == 0) {
                //Do not draw tiles that do not emit light at all.
                //不绘制完全不发光的瓦片。
                continue;
            }
#else
            if (config->light.enable) {
                if (finalLightColor == nullptr) {
                    continue;
                }
                if (finalLightColor->a == 0) {
                    //Do not draw tiles that do not emit light at all.
                    //不绘制完全不发光的瓦片。
                    continue;
                }
            }
#endif
            SDL_Texture *texture = tile->GetTexture();
            if (texture != nullptr) {
                SDL_SetTextureAlphaMod(texture, alpha);
                if (!SDL_RenderTexture(renderer, texture, nullptr, &renderQuad)) {
                    LogCat::e("SDL_RenderTexture Error: ", SDL_GetError());
                }
                SDL_SetTextureAlphaMod(texture, 255);
            }
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

uint8_t glimmer::TileLayerSystem::GetRenderOrder() {
    return RENDER_ORDER_TILE_LAYER;
}

bool glimmer::TileLayerSystem::HandleEvent(const SDL_Event &event) {
    if (worldContext_ == nullptr) {
        return false;
    }
    if (event.type != SDL_EVENT_MOUSE_MOTION) {
        return false;
    }
    const auto *camera = worldContext_->GetCameraComponent();
    if (camera == nullptr) {
        return false;
    }
    const auto *cameraTransform = worldContext_->GetCameraTransform2D();
    if (cameraTransform == nullptr) {
        return false;
    }
    const WorldVector2D worldPos = camera->GetWorldPosition(
        cameraTransform->GetPosition(),
        CameraVector2D(event.motion.x, event.motion.y)
    );
    auto tileLayerEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    for (auto &entity: tileLayerEntities) {
        auto *layer = worldContext_->GetComponent<TileLayerComponent>(entity);
        if (layer == nullptr) {
            continue;
        }
        layer->SetFocusPosition(TileLayerComponent::WorldToTile(worldPos));
    }
    return true;
}

std::string glimmer::TileLayerSystem::GetName() {
    return "glimmer.TileLayerSystem";
}
