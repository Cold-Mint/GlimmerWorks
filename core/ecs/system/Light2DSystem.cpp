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

#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/WindowContext.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/LightBuffer.h"
#include "core/world/WorldContext.h"

void glimmer::Light2DSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    EntityShortCut *entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr) {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
}

uint8_t glimmer::Light2DSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_LIGHT2D;
}

glimmer::Light2DSystem::Light2DSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

void glimmer::Light2DSystem::Render(RenderQueue *) {
//     WorldContext *worldContext = GetWorldContext();
//     if (worldContext == nullptr) {
//         return;
//     }
//     const AppContext *appContext = worldContext->GetAppContext();
//     if (appContext == nullptr) {
//         return;
//     }
//     const Config *config = appContext->GetConfig();
//     if (config == nullptr) {
//         return;
//     }
// #if  !defined(NDEBUG)
//     if (!config->light.enable) {
//         return;
//     }
// #endif
//     if (cameraComponent_ == nullptr) {
//         return;
//     }
//     if (cameraTransform2DComponent_ == nullptr) {
//         return;
//     }
//     WindowContext *windowContext = appContext->GetWindowContext();
//     if (windowContext == nullptr) {
//         return;
//     }
//     GpuRenderer *renderer = windowContext->GetRenderer();
//     if (renderer == nullptr) {
//         return;
//     }
//     LightBuffer *lightBuffer = worldContext->GetLightingBuffer();
//     if (lightBuffer == nullptr) {
//         return;
//     }
//     const float zoom = cameraComponent_->GetZoom();
//     const SDL_FRect viewportRect = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
//                                                                           cameraComponent_->GetSize(), zoom);
//     //Covered tile area: the viewport plus one extra tile at the far edges to
//     //prevent blank borders (world +Y points up, so rect.y is the bottom edge).
//     //覆盖的瓦片区域：视口外加远侧边缘各一格，防止出现空白边界
//     //（世界 Y 轴向上，因此 rect.y 是底边缘）。
//     const TileVector2D leftBottom = CoordinateTransformer::WorldToTile({viewportRect.x, viewportRect.y});
//     const TileVector2D rightTop = CoordinateTransformer::WorldToTile({
//         viewportRect.x + viewportRect.w + TILE_SIZE,
//         viewportRect.y + viewportRect.h + TILE_SIZE
//     });
//     const int width = rightTop.x - leftBottom.x + 1;
//     const int height = rightTop.y - leftBottom.y + 1;
//     if (width <= 0 || height <= 0) {
//         return;
//     }
//     LightMapParams params{};
//     //Texel (0,0) of the light map covers the left/top tile of the area.
//     //光照贴图的 texel (0,0) 覆盖区域的左/上瓦片。
//     params.lightMapOriginX = static_cast<float>(leftBottom.x);
//     params.lightMapOriginY = static_cast<float>(rightTop.y);
//     params.lightMapSizeX = static_cast<float>(width);
//     params.lightMapSizeY = static_cast<float>(height);
//     const float tileSize = static_cast<float>(TILE_SIZE);
//     params.cameraTopLeftTileX = viewportRect.x / tileSize;
//     params.cameraTopLeftTileY = (viewportRect.y + viewportRect.h) / tileSize;
//     params.viewportTilesX = viewportRect.w / tileSize;
//     params.viewportTilesY = viewportRect.h / tileSize;
//     params.fullBright = config->lighting.fullBrightAlpha / 255.0F;
//     params.minVisibility = config->lighting.minVisibility;
//     params.tintStrength = config->lighting.tintStrength;
//
//     const uint64_t revision = lightBuffer->GetRevision();
//     const bool rebuild = !hasCache_ || width != lastWidth_ || height != lastHeight_ ||
//                          leftBottom.x != lastOriginX_ || rightTop.y != lastOriginY_ || revision != lastRevision_;
//     if (!rebuild) {
//         //The camera moved inside the same tile area and the light data did
//         //not change: keep the GPU texture, only refresh the shader params.
//         //相机在同一瓦片区域内移动且光照数据未变化：保留 GPU 纹理，仅刷新着色器参数。
//         renderer->SetLightMap(width, height, nullptr, params);
//         return;
//     }
//     pixelBuffer_.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
//     for (int row = 0; row < height; ++row) {
//         //Texture row 0 is the top of the screen; world +Y points up, so rows
//         //walk downwards from the highest tile y.
//         //纹理第 0 行是屏幕顶部；世界 Y 轴向上，因此行从最高的瓦片 y 向下遍历。
//         const int tileY = rightTop.y - row;
//         for (int column = 0; column < width; ++column) {
//             const int tileX = leftBottom.x + column;
//             uint8_t *pixel = pixelBuffer_.data() + (static_cast<size_t>(row) * static_cast<size_t>(width) +
//                                                     static_cast<size_t>(column)) * 4;
//             const Color *finalLightColor = lightBuffer->GetFinalLightColor(TileVector2D(tileX, tileY));
//             if (finalLightColor == nullptr) {
//                 pixel[0] = 0;
//                 pixel[1] = 0;
//                 pixel[2] = 0;
//                 pixel[3] = 0;
//             } else {
//                 pixel[0] = finalLightColor->r;
//                 pixel[1] = finalLightColor->g;
//                 pixel[2] = finalLightColor->b;
//                 pixel[3] = finalLightColor->a;
//             }
//         }
//     }
//     lastWidth_ = width;
//     lastHeight_ = height;
//     lastOriginX_ = leftBottom.x;
//     lastOriginY_ = rightTop.y;
//     lastRevision_ = revision;
//     hasCache_ = true;
//     renderer->SetLightMap(width, height, pixelBuffer_.data(), params);
}

glimmer::GameSystemType glimmer::Light2DSystem::GetGameSystemType() const {
    return GameSystemType::Light2DSystem;
}
