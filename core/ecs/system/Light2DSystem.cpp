//
// Created by coldmint on 2026/4/10.
//

#include "Light2DSystem.h"

#include "core/Constants.h"
#include "core/utils/ColorUtils.h"
#include "core/world/LightPropagationTraverser.h"
#include "core/world/WorldContext.h"


std::vector<glimmer::Tile *> glimmer::Light2DSystem::GetTopVisibleTiles(TileVector2D tileVector2d) const {
    if (worldContext_ == nullptr) {
        return {};
    }
    const Chunk *chunk = worldContext_->GetChunk(Chunk::TileCoordinatesToChunkVertexCoordinates(tileVector2d));
    if (chunk == nullptr) {
        return {};
    }
    return chunk->GetTopVisibleTiles(Ground | BackGround,
                                     Chunk::TileCoordinatesToChunkRelativeCoordinates(tileVector2d));
}

glimmer::TraverseAction glimmer::Light2DSystem::StepCallback(const Tile *centerTile, const TileVector2D current,
                                                             TileVector2D next, const float distance) {
    const float ratio = distance / static_cast<float>(centerTile->GetEmissionRadius());
    const float decay = centerTile->IsLightPeakAtCenter() ? 1.0F - ratio : ratio;
    const float finalBrightness = std::max(decay, centerTile->GetMinLightBrightness());
    const SDL_Color emissionColor = centerTile->GetEmissionColor();
    auto nextTiles = GetTopVisibleTiles(next);
    for (auto &tile: nextTiles) {
        tile->SetLightColor(ColorUtils::DecayColor(emissionColor, finalBrightness));
    }
    return TraverseAction::Continue;
}


glimmer::Light2DSystem::Light2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent<TileLayerComponent>();
}

void glimmer::Light2DSystem::Update(float delta) {
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
    CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    if (cameraComponent == nullptr) {
        return;
    }
    Transform2DComponent *cameraTransform2DComponent = worldContext_->GetCameraTransform2D();
    if (cameraTransform2DComponent == nullptr) {
        return;
    }
    SDL_FRect viewRect = cameraComponent->GetViewportRect(cameraTransform2DComponent->GetPosition());
    //The complete range of pre-calculated blocks.
    //预计算的完整区块范围。
    auto preloadedLightingViewportRect = viewRect;
    const float preloadLightingRadius = config->world.preloadLightingRadius;
    constexpr float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    preloadedLightingViewportRect.x -= preloadLightingRadius * chunkWorldSize;
    preloadedLightingViewportRect.y -= preloadLightingRadius * chunkWorldSize;
    preloadedLightingViewportRect.w += preloadLightingRadius * 2 * chunkWorldSize;
    preloadedLightingViewportRect.h += preloadLightingRadius * 2 * chunkWorldSize;
    TileVector2D topLeftLightingCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedLightingViewportRect.x, preloadedLightingViewportRect.y));
    TileVector2D lowerRightLightingCorner = TileLayerComponent::WorldToTile(
        WorldVector2D(preloadedLightingViewportRect.x + preloadedLightingViewportRect.w,
                      preloadedLightingViewportRect.y + preloadedLightingViewportRect.h));
    const TileVector2D startChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(topLeftLightingCorner);
    const TileVector2D endChunk = Chunk::TileCoordinatesToChunkVertexCoordinates(lowerRightLightingCorner);
    for (int cy = startChunk.y; cy <= endChunk.y; cy += CHUNK_SIZE) {
        for (int cx = startChunk.x; cx <= endChunk.x; cx += CHUNK_SIZE) {
            TileVector2D chunkPos(cx, cy);
            if (!WorldContext::ChunkIsOutOfBounds(chunkPos) && worldContext_->HasChunk(chunkPos)) {
                const Chunk *chunk = worldContext_->GetChunk(chunkPos);
                if (chunk == nullptr) {
                    continue;
                }
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        std::vector<Tile *> tiles = chunk->GetTopVisibleTiles(
                            Ground | BackGround, TileVector2D{x, y});
                        if (tiles.empty()) {
                            continue;
                        }
                        for (auto &tile: tiles) {
                            tile->SetLightColor(tile->GetEmissionColor());
                        }
                    }
                }

                for (int y = 0; y < CHUNK_SIZE; y++) {
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        std::vector<Tile *> tiles = chunk->GetTopVisibleTiles(
                            Ground | BackGround, TileVector2D{x, y});
                        if (tiles.empty()) {
                            continue;
                        }
                        for (auto &tile: tiles) {
                            const SDL_Color emissionColor = tile->GetEmissionColor();
                            if (emissionColor.a == 0 || tile->GetEmissionRadius() <= 0) {
                                //If the luminous intensity is 0, or the luminous radius is 0.
                                //如果发光强度为0，或者发光半径为0。
                                continue;
                            }
                            LightPropagationTraverser radial8WayClockwiseTraverser = {
                                tile->GetEmissionRadius(),
                                [this, tile](const TileVector2D cur,
                                             const TileVector2D next, const float distance) {
                                    return this->StepCallback(tile, cur, next, distance);
                                },
                                TileVector2D{cx + x, cy + y}
                            };
                            radial8WayClockwiseTraverser.Start();
                            tile->SetLightColor(tile->GetEmissionColor());
                        }
                    }
                }
            }
        }
    }
}

uint8_t glimmer::Light2DSystem::GetRenderOrder() {
    return RENDER_ORDER_LIGHT2D;
}

void glimmer::Light2DSystem::Render(SDL_Renderer *renderer) {
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
    if (!config->debug.enableLighting) {
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
    std::vector<std::pair<TileVector2D, std::vector<Tile *> > > visibleTiles =
            tileLayerComponent->GetTopVisibleTilesInViewport(Ground | BackGround, viewportRect);
    for (const auto &[tileCoord, tileList]: visibleTiles) {
        const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileCoord);
        const CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
            cameraPos->GetPosition(), worldTilePos);
        SDL_FRect renderQuad;
        renderQuad.w = TILE_SIZE * zoom;
        renderQuad.h = TILE_SIZE * zoom;
        renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
        renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
        SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
        for (auto tile: tileList) {
            const SDL_Color lightColor = tile->GetLightColor();
            if (lightColor.a == 0) {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, lightColor.r,
                                   lightColor.g,
                                   lightColor.b,
                                   lightColor.a);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

std::string glimmer::Light2DSystem::GetName() {
    return "glimmer.Light2dSystem";
}
