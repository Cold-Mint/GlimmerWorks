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
#include "DebugChunkSystem.h"

#include <cmath>

#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/gpu/RenderQueue.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/ChunkManager.h"
#include "core/world/WorldContext.h"

glimmer::DebugChunkSystem::DebugChunkSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    Init();
}

bool glimmer::DebugChunkSystem::CanActive() const {
    return displayChunkView_;
}

void glimmer::DebugChunkSystem::OnConfigChanged(const Config *config) {
    displayChunkView_ = config->debug.displayChunkView;
}

void glimmer::DebugChunkSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count) {
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr) {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
}

void glimmer::DebugChunkSystem::RenderChunkBounds(RenderQueue *queue) {
    auto viewportRect = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                               cameraComponent_->GetSize(),
                                                               cameraComponent_->GetZoom());
    const float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    const int minChunkX = static_cast<int>(std::floor(viewportRect.x / chunkWorldSize));
    const int minChunkY = static_cast<int>(std::floor(viewportRect.y / chunkWorldSize));
    const int maxChunkX = static_cast<int>(std::floor((viewportRect.x + viewportRect.w) / chunkWorldSize));
    const int maxChunkY = static_cast<int>(std::floor((viewportRect.y + viewportRect.h) / chunkWorldSize));
    const SDL_Color chunkBorderColor = {0, 255, 255, 180};
    for (int chunkX = minChunkX; chunkX <= maxChunkX; ++chunkX) {
        for (int chunkY = minChunkY; chunkY <= maxChunkY; ++chunkY) {
            WorldVector2D chunkWorldPos{
                static_cast<float>(chunkX) * chunkWorldSize - HALF_TILE_SIZE,
                static_cast<float>(chunkY) * chunkWorldSize - HALF_TILE_SIZE
            };
            const ScreenVector2D screenPos = CoordinateTransformer::WorldToScreen(
                cameraTransform2DComponent_->GetPosition(), chunkWorldPos,
                cameraComponent_->GetSize(), cameraComponent_->GetZoom());
            const float pixelSize = chunkWorldSize * cameraComponent_->GetZoom();
            const SDL_FRect rect{
                screenPos.x,
                screenPos.y - pixelSize,
                pixelSize,
                pixelSize
            };
            queue->DrawRect(RenderLayer::Debug, 0.0F, &rect, chunkBorderColor);
        }
    }
}

void glimmer::DebugChunkSystem::RenderChunkView(RenderQueue *queue, AppContext *appContext) {
    const WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const auto *chunksPtr = worldContext->GetChunkManager()->GetAllChunks();
    if (chunksPtr == nullptr) {
        return;
    }
    const WindowContext *windowContext = appContext->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    const int windowHeight = windowContext->GetWindowHeight();

    const auto getChunkIndex = [](const int tileCoord) {
        return static_cast<int>(std::floor(static_cast<float>(tileCoord) / CHUNK_SIZE));
    };

    const int playerTileX = static_cast<int>(std::floor(mousePosition_.x / TILE_SIZE));
    const int playerTileY = static_cast<int>(std::floor(mousePosition_.y / TILE_SIZE));
    const int playerChunkX = getChunkIndex(playerTileX);
    const int playerChunkY = getChunkIndex(playerTileY);

    constexpr float cellSize = 10.0F;
    constexpr float gridCenterX = 100.0F;
    const float gridCenterY = static_cast<float>(windowHeight) - 100.0F;

    // Draw Loaded Chunks (Blue)
    // 绘制已加载的区块（蓝色）
    const SDL_Color loadedChunkColor = {100, 149, 237, 128};
    for (const auto &[pos, chunk]: *chunksPtr) {
        const int chunkIndexX = pos.x >> CHUNK_SHIFT;
        const int chunkIndexY = pos.y >> CHUNK_SHIFT;
        const float drawX = gridCenterX + static_cast<float>(chunkIndexX - playerChunkX) * cellSize;
        const float drawY = gridCenterY + static_cast<float>(playerChunkY - chunkIndexY) * cellSize;
        const SDL_FRect rect = {drawX, drawY, cellSize - 1.0F, cellSize - 1.0F};
        queue->FillRect(RenderLayer::Debug, 0.0F, &rect, loadedChunkColor);
    }

    // Draw Current Chunk (Red)
    // 绘制当前区块（红色）
    const SDL_Color currentChunkColor = {255, 69, 0, 200};
    const SDL_FRect playerRect = {gridCenterX, gridCenterY, cellSize - 1.0F, cellSize - 1.0F};
    queue->FillRect(RenderLayer::Debug, 0.0F, &playerRect, currentChunkColor);

    // Draw Visible Chunks (Orange)
    // 绘制可见区块（橙色）
    const SDL_FRect viewport = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                                      cameraComponent_->GetSize(),
                                                                      cameraComponent_->GetZoom());
    const int startChunkX = static_cast<int>(std::floor(viewport.x / TILE_SIZE / CHUNK_SIZE));
    const int startChunkY = static_cast<int>(std::floor(viewport.y / TILE_SIZE / CHUNK_SIZE));
    const int endChunkX = static_cast<int>(std::floor((viewport.x + viewport.w) / TILE_SIZE / CHUNK_SIZE));
    const int endChunkY = static_cast<int>(std::floor((viewport.y + viewport.h) / TILE_SIZE / CHUNK_SIZE));
    const SDL_Color visibleChunkColor = {255, 165, 0, 255};
    for (int cy = startChunkY; cy <= endChunkY; ++cy) {
        for (int cx = startChunkX; cx <= endChunkX; ++cx) {
            const float drawX = gridCenterX + static_cast<float>(cx - playerChunkX) * cellSize;
            const float drawY = gridCenterY + static_cast<float>(playerChunkY - cy) * cellSize;
            const SDL_FRect top = {drawX, drawY, cellSize, 1.0F};
            queue->FillRect(RenderLayer::Debug, 0.0F, &top, visibleChunkColor);
            const SDL_FRect bottom = {drawX, drawY + cellSize - 1.0F, cellSize, 1.0F};
            queue->FillRect(RenderLayer::Debug, 0.0F, &bottom, visibleChunkColor);
            const SDL_FRect left = {drawX, drawY, 1.0F, cellSize};
            queue->FillRect(RenderLayer::Debug, 0.0F, &left, visibleChunkColor);
            const SDL_FRect right = {drawX + cellSize - 1.0F, drawY, 1.0F, cellSize};
            queue->FillRect(RenderLayer::Debug, 0.0F, &right, visibleChunkColor);
        }
    }
}

void glimmer::DebugChunkSystem::Render(RenderQueue *queue) {
    if (cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr) {
        return;
    }
    const WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    RenderChunkBounds(queue);
    RenderChunkView(queue, appContext);
}

bool glimmer::DebugChunkSystem::HandleEvent(const SDL_Event &event) {
    if (cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mousePosition_ = CoordinateTransformer::ScreenToWorld(
            cameraTransform2DComponent_->GetPosition(),
            ScreenVector2D{
                event.motion.x, event.motion.y
            },
            cameraComponent_->GetSize(),
            cameraComponent_->GetZoom());
    }
    return false;
}

uint8_t glimmer::DebugChunkSystem::GetExecutionOrder() {
    return EXECUTION_ORDER_DEBUG_CHUNK;
}

glimmer::GameSystemType glimmer::DebugChunkSystem::GetGameSystemType() const {
    return GameSystemType::DebugChunkSystem;
}

#endif
