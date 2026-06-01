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
#include "Light2DSystem.h"

#include "core/Constants.h"
#include "core/utils/ColorUtils.h"
#include "core/world/LightPropagationTraverser.h"
#include "core/world/WorldContext.h"

glimmer::Light2DSystem::Light2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    RequireComponent(COMPONENT_TILE_LAYER);
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
    #if  !defined(NDEBUG)
    const Config *config = appContext->GetConfig();
    if (config == nullptr) {
        return;
    }
    if (!config->light.enable) {
        return;
    }
    #endif
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
    const TileVector2D topLeft = TileLayerComponent::WorldToTile({viewportRect.x, viewportRect.y});
    //The purpose of adding "TILE_SIZE" in the lower right corner is to prevent blank areas from appearing.
    //右下角加TILE_SIZE的目的是，防止出现空白区域。
    const TileVector2D bottomRight = TileLayerComponent::WorldToTile({
        viewportRect.x + viewportRect.w + TILE_SIZE,
        viewportRect.y + viewportRect.h + TILE_SIZE
    });
    for (int y = topLeft.y; y <= bottomRight.y; ++y) {
        for (int x = topLeft.x; x <= bottomRight.x; ++x) {
            auto tileVector2D = TileVector2D(x, y);
            const WorldVector2D worldTilePos = TileLayerComponent::TileToWorld(tileVector2D);
            const CameraVector2D screenPos = cameraComponent->WorldToScreen(
                cameraPos->GetPosition(), worldTilePos);
            SDL_FRect renderQuad;
            renderQuad.w = TILE_SIZE * zoom;
            renderQuad.h = TILE_SIZE * zoom;
            renderQuad.x = screenPos.x - renderQuad.w * 0.5F;
            renderQuad.y = screenPos.y - renderQuad.h * 0.5F;
            SDL_FRect dstRect = {renderQuad.x, renderQuad.y, renderQuad.w, renderQuad.h};
            const Color *finalLightColor = worldContext_->GetLightingBuffer()->GetFinalLightColor(tileVector2D);
            if (finalLightColor == nullptr) {
                continue;
            }
            if (finalLightColor->a == 0) {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, finalLightColor->r,
                                   finalLightColor->g,
                                   finalLightColor->b,
                                   finalLightColor->a);
            SDL_RenderFillRect(renderer, &dstRect);
        }
    }
    AppContext::RestoreColorRenderer(renderer);
}

std::string glimmer::Light2DSystem::GetName() {
    return "glimmer.Light2dSystem";
}
