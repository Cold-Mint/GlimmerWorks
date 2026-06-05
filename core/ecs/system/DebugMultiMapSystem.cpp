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
#if  !defined(NDEBUG)
#include "DebugMultiMapSystem.h"

#include "core/scene/AppContext.h"
#include "core/utils/ColorUtils.h"
#include "core/world/WorldContext.h"

glimmer::DebugMultiMapSystem::DebugMultiMapSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
}

glimmer::GameSystemType glimmer::DebugMultiMapSystem::GetGameSystemType() const
{
    return GameSystemType::DebugMultiMapSystem;
}

glimmer::Color glimmer::DebugMultiMapSystem::GetTileDebugColor(const TileVector2D tile) const
{
    constexpr Color color = {0, 0, 0, 0};
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext == nullptr)
    {
        return color;
    }
    const Config* config = appContext->GetConfig();
    if (config == nullptr)
    {
        return color;
    }
    auto debugColor = appContext->GetPreloadColors()->debugColor;
    float elevation = ChunkGenerator::GetElevation(tile.x);
    ChunkGenerator* chunkGenerator = worldContext_->GetChunkGenerator();
    std::vector<Color> activeColors;

    if (config->debug.displayElevationMap)
    {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.elevationMapFrom, debugColor.elevationMapTo, elevation)
        );
    }
    if (config->debug.displayTempMap)
    {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.tempMapFrom, debugColor.tempMapTo,
                                  chunkGenerator->GetTemperature(tile, elevation))
        );
    }
    if (config->debug.displayHumidityMap)
    {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.humidityMapFrom, debugColor.humidityMapTo,
                                  chunkGenerator->GetHumidity(tile))
        );
    }
    if (config->debug.displayErosionMap)
    {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.erosionMapFrom, debugColor.erosionMapTo,
                                  chunkGenerator->GetErosion(tile))
        );
    }
    if (config->debug.displayWeirdnessMap)
    {
        activeColors.emplace_back(
            ColorUtils::LerpColor(debugColor.weirdnessMapFrom, debugColor.weirdnessMapTo,
                                  chunkGenerator->GetWeirdness(tile))
        );
    }
    return ColorUtils::AverageColors(activeColors);
}

void glimmer::DebugMultiMapSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
}

uint8_t glimmer::DebugMultiMapSystem::GetRenderOrder()
{
    return RENDER_ORDER_DEBUG_MAP;
}

void glimmer::DebugMultiMapSystem::Render(SDL_Renderer* renderer)
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
    auto viewportRect = cameraComponent_->GetViewportRect(cameraTransform2DComponent_->GetPosition());
    const float zoom = cameraComponent_->GetZoom();
    const TileVector2D topLeft = TileLayerComponent::WorldToTile({viewportRect.x, viewportRect.y});
    const TileVector2D bottomRight = TileLayerComponent::WorldToTile({
        viewportRect.x + viewportRect.w + TILE_SIZE,
        viewportRect.y + viewportRect.h + TILE_SIZE
    });

    for (int x = topLeft.x; x < bottomRight.x; x++)
    {
        for (int y = topLeft.y; y < bottomRight.y; y++)
        {
            const TileVector2D tileVector2D = {x, y};
            const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileVector2D);
            const CameraVector2D screenPos = cameraComponent_->WorldToScreen(
                cameraTransform2DComponent_->GetPosition(), worldTilePos);
            SDL_FRect renderQuad;
            renderQuad.w = TILE_SIZE * zoom;
            renderQuad.h = TILE_SIZE * zoom;
            renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
            renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
            SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
            const Color color = GetTileDebugColor(tileVector2D);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }

    AppContext::RestoreColorRenderer(renderer);
}
#endif
