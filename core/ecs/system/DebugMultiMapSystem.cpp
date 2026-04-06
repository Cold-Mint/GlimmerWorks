//
// Created by coldmint on 2026/4/4.
//

#include "DebugMultiMapSystem.h"

#include "core/scene/AppContext.h"
#include "core/utils/ColorUtils.h"
#include "core/world/WorldContext.h"

glimmer::DebugMultiMapSystem::DebugMultiMapSystem(WorldContext *worldContext) : GameSystem(worldContext) {
}

bool glimmer::DebugMultiMapSystem::ShouldActivate() {
    if (worldContext_ == nullptr) {
        return false;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return false;
    }
    return config->debug.displayElevationMap || config->debug.displayTempMap || config->debug.displayHumidityMap ||
           config->debug.displayErosionMap || config->debug.displayWeirdnessMap;
}

SDL_Color glimmer::DebugMultiMapSystem::GetTileDebugColor(TileVector2D tile) const {
    SDL_Color color = {0, 0, 0, 0};
    if (worldContext_ == nullptr) {
        return color;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return color;
    }
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return color;
    }
    auto debugColor = appContext->GetPreloadColors()->debugColor;
    float elevation = ChunkGenerator::GetElevation(tile.x);
    ChunkGenerator *chunkGenerator = worldContext_->GetChunkGenerator();
    std::vector<SDL_Color> activeColors;

    if (config->debug.displayElevationMap) {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.elevationMapFrom, debugColor.elevationMapTo, elevation)
        );
    }
    if (config->debug.displayTempMap) {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.tempMapFrom, debugColor.tempMapTo,
                                  chunkGenerator->GetTemperature(tile, elevation))
        );
    }
    if (config->debug.displayHumidityMap) {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.humidityMapFrom, debugColor.humidityMapTo,
                                  chunkGenerator->GetHumidity(tile))
        );
    }
    if (config->debug.displayErosionMap) {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.erosionMapFrom, debugColor.erosionMapTo,
                                  chunkGenerator->GetErosion(tile))
        );
    }
    if (config->debug.displayWeirdnessMap) {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.weirdnessMapFrom, debugColor.weirdnessMapTo,
                                  chunkGenerator->GetWeirdness(tile))
        );
    }
    return ColorUtils::AverageColors(activeColors);
}

uint8_t glimmer::DebugMultiMapSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_MAP;
}

void glimmer::DebugMultiMapSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_ == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    const auto *cameraComponent = worldContext_->GetCameraComponent();
    if (cameraComponent == nullptr) {
        return;
    }
    const auto *cameraPos = worldContext_->GetCameraTransform2D();
    if (cameraPos == nullptr) {
        return;
    }
    auto gameEntities = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();

    auto viewportRect = cameraComponent->GetViewportRect(cameraPos->GetPosition());
    const float zoom = cameraComponent->GetZoom();
    const TileVector2D topLeft = TileLayerComponent::WorldToTile({viewportRect.x, viewportRect.y});
    const TileVector2D bottomRight = TileLayerComponent::WorldToTile({
        viewportRect.x + viewportRect.w + TILE_SIZE,
        viewportRect.y + viewportRect.h + TILE_SIZE
    });

    for (int x = topLeft.x; x < bottomRight.x; x++) {
        for (int y = topLeft.y; y < bottomRight.y; y++) {
            const TileVector2D tileVector2D = {x, y};
            const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileVector2D);
            const CameraVector2D screenPos = cameraComponent->GetViewPortPosition(
                cameraPos->GetPosition(), worldTilePos);
            SDL_FRect renderQuad;
            renderQuad.w = TILE_SIZE * zoom;
            renderQuad.h = TILE_SIZE * zoom;
            renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
            renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
            SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
            const SDL_Color color = GetTileDebugColor(tileVector2D);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }

    AppContext::RestoreColorRenderer(renderer);
}

std::string glimmer::DebugMultiMapSystem::GetName() {
    return "DebugMultiMapSystem";
}
