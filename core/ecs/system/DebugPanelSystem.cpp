//
// Created by Cold-Mint on 2025/11/3.
//

#include "DebugPanelSystem.h"
#include <cmath>

#include "../../Constants.h"
#include "../component/PlayerControlComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"


bool glimmer::DebugPanelSystem::ShouldActivate() {
    return appContext_->isDebugMode();
}

void glimmer::DebugPanelSystem::Render(SDL_Renderer *renderer) {
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    int windowW = 0, windowH = 0;
    SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) return;

    float yOffset = 0.0f;

    for (auto &entity: entities) {
        constexpr float lineSpacing = 20.0f;
        SDL_Color color = {255, 255, 180, 255};
        auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
        auto position = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
        if (!control || !position) continue;

        const WorldVector2D playerPos = position->GetPosition();

        // 先统计总行数（玩家 + 所有 TileLayer）
        auto tileLayers = worldContext_->GetEntitiesWithComponents<TileLayerComponent, Transform2DComponent>();
        float totalLines = 1.0F + static_cast<float>(tileLayers.size());
        float totalTextHeight = totalLines * lineSpacing;

        // 起始 y 坐标垂直居中
        yOffset = (static_cast<float>(windowH) - totalTextHeight) / 2.0f;

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Player World: (%.1f, %.1f)", playerPos.x, playerPos.y);

        SDL_Surface *surface = TTF_RenderText_Blended(appContext_->ttfFont, buffer, strlen(buffer), color);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_FRect dst = {
                    static_cast<float>(windowW - surface->w - 4), // 右侧靠边
                    yOffset,
                    static_cast<float>(surface->w),
                    static_cast<float>(surface->h)
                };
                SDL_RenderTexture(renderer, texture, nullptr, &dst);
                SDL_DestroyTexture(texture);
            }
            SDL_DestroySurface(surface);
        }

        yOffset += lineSpacing;

        // 遍历 TileLayer 并打印瓦片坐标
        for (auto &tileEntity: tileLayers) {
            auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(tileEntity->GetID());
            auto layerPos = worldContext_->GetComponent<Transform2DComponent>(tileEntity->GetID());
            if (!tileLayer || !layerPos) continue;

            TileVector2D tileCoord = tileLayer->WorldToTile(layerPos->GetPosition(), playerPos);
            snprintf(buffer, sizeof(buffer), "Tile Coord: (%d, %d)", tileCoord.x, tileCoord.y);

            SDL_Surface *s = TTF_RenderText_Blended(appContext_->ttfFont, buffer, strlen(buffer), color);
            if (s) {
                SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
                if (t) {
                    SDL_FRect dst = {
                        static_cast<float>(windowW - s->w - 4), // 右侧靠边
                        yOffset,
                        static_cast<float>(s->w),
                        static_cast<float>(s->h)
                    };
                    SDL_RenderTexture(renderer, t, nullptr, &dst);
                    SDL_DestroyTexture(t);
                }
                SDL_DestroySurface(s);
            }

            yOffset += lineSpacing;
        }
    }

    SDL_Color oldColor;
    SDL_GetRenderDrawColor(renderer, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

    // 绿色
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    const float cx = static_cast<float>(windowW) * 0.5f;
    const float cy = static_cast<float>(windowH) * 0.5f;

    // 线宽 3px（用填充矩形实现以保证宽度）
    constexpr float lineWidth = 3.0f;

    // 垂直线（满高）
    SDL_FRect vert = {
        cx - lineWidth * 0.5f, // x
        0.0f, // y
        lineWidth, // w
        static_cast<float>(windowH) // h
    };
    SDL_RenderFillRect(renderer, &vert);

    // 水平线（满宽）
    SDL_FRect horz = {
        0.0f, // x
        cy - lineWidth * 0.5f, // y
        static_cast<float>(windowW), // w
        lineWidth // h
    };
    SDL_RenderFillRect(renderer, &horz);

    // 中心绿色点（稍大些以便可见）
    const float pointSize = 6.0f;
    SDL_FRect point = {
        cx - pointSize * 0.5f,
        cy - pointSize * 0.5f,
        pointSize,
        pointSize
    };
    SDL_RenderFillRect(renderer, &point);

    // Draw Chunk Grid in Bottom-Left
    // 在左下角绘制区块网格
    const auto &chunks = worldContext_->GetAllChunks();
    if (!entities.empty()) {
        auto firstPlayerEntity = entities[0];
        auto position = worldContext_->GetComponent<Transform2DComponent>(firstPlayerEntity->GetID());
        if (position) {
            WorldVector2D playerPos = position->GetPosition();
            int playerTileX = static_cast<int>(std::floor(playerPos.x / TILE_SIZE));
            int playerTileY = static_cast<int>(std::floor(playerPos.y / TILE_SIZE));

            auto getChunkIndex = [](int tileCoord) {
                return static_cast<int>(std::floor(static_cast<float>(tileCoord) / CHUNK_SIZE));
            };

            int playerChunkX = getChunkIndex(playerTileX);
            int playerChunkY = getChunkIndex(playerTileY);

            float cellSize = 10.0f;
            float gridCenterX = 100.0f;
            float gridCenterY = static_cast<float>(windowH) - 100.0f;

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

            // Draw Loaded Chunks (Blue)
            // 绘制已加载的区块（蓝色）
            SDL_SetRenderDrawColor(renderer, 100, 149, 237, 128);

            for (const auto &[pos, chunk]: chunks) {
                int chunkIndexX = pos.x / CHUNK_SIZE;
                int chunkIndexY = pos.y / CHUNK_SIZE;

                float drawX = gridCenterX + (chunkIndexX - playerChunkX) * cellSize;
                float drawY = gridCenterY + (chunkIndexY - playerChunkY) * cellSize;

                SDL_FRect rect = {drawX, drawY, cellSize - 1.0f, cellSize - 1.0f};
                SDL_RenderFillRect(renderer, &rect);
            }

            // Draw Current Chunk (Red)
            // 绘制当前区块（红色）
            SDL_SetRenderDrawColor(renderer, 255, 69, 0, 200);
            SDL_FRect playerRect = {gridCenterX, gridCenterY, cellSize - 1.0f, cellSize - 1.0f};
            SDL_RenderFillRect(renderer, &playerRect);

            // Draw Visible Chunks (Orange)
            // 绘制可见区块（橙色）
            auto camera = worldContext_->GetCameraComponent();
            auto cameraTransform = worldContext_->GetCameraTransform2D();
            int visibleChunkCount = 0;
            if (camera && cameraTransform) {
                SDL_FRect viewport = camera->GetViewportRect(cameraTransform->GetPosition());

                // Calculate visible chunk range
                // 计算可见区块范围
                int startChunkX = static_cast<int>(std::floor(viewport.x / TILE_SIZE / CHUNK_SIZE));
                int startChunkY = static_cast<int>(std::floor(viewport.y / TILE_SIZE / CHUNK_SIZE));
                int endChunkX = static_cast<int>(std::floor((viewport.x + viewport.w) / TILE_SIZE / CHUNK_SIZE));
                int endChunkY = static_cast<int>(std::floor((viewport.y + viewport.h) / TILE_SIZE / CHUNK_SIZE));

                visibleChunkCount = (endChunkX - startChunkX + 1) * (endChunkY - startChunkY + 1);

                SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange

                for (int cy = startChunkY; cy <= endChunkY; ++cy) {
                    for (int cx = startChunkX; cx <= endChunkX; ++cx) {
                        float drawX = gridCenterX + (cx - playerChunkX) * cellSize;
                        float drawY = gridCenterY + (cy - playerChunkY) * cellSize;


                        SDL_FRect top = {drawX, drawY, cellSize, 1.0f};
                        SDL_RenderFillRect(renderer, &top);
                        SDL_FRect bottom = {drawX, drawY + cellSize - 1.0f, cellSize, 1.0f};
                        SDL_RenderFillRect(renderer, &bottom);
                        SDL_FRect left = {drawX, drawY, 1.0f, cellSize};
                        SDL_RenderFillRect(renderer, &left);
                        SDL_FRect right = {drawX + cellSize - 1.0f, drawY, 1.0f, cellSize};
                        SDL_RenderFillRect(renderer, &right);
                    }
                }
            }

            // Draw Chunk Info Text
            // 绘制区块信息文本
            char chunkText[128];
            snprintf(chunkText, sizeof(chunkText), "Chunk: (%d, %d) | Vis: %d | Total: %zu",
                     playerChunkX, playerChunkY, visibleChunkCount, chunks.size());
            SDL_Surface *s = TTF_RenderText_Blended(appContext_->ttfFont, chunkText, strlen(chunkText),
                                                    {255, 255, 255, 255});
            if (s) {
                SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
                if (t) {
                    SDL_FRect dst = {
                        48.0F,
                        static_cast<float>(windowH) - static_cast<float>(s->h) - 8.0F,
                        static_cast<float>(s->w),
                        static_cast<float>(s->h)
                    };
                    SDL_RenderTexture(renderer, t, nullptr, &dst);
                    SDL_DestroyTexture(t);
                }
                SDL_DestroySurface(s);
            }
        }
    }

    // 恢复原始颜色
    SDL_SetRenderDrawColor(renderer, oldColor.r, oldColor.g, oldColor.b, oldColor.a);
}

uint8_t glimmer::DebugPanelSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_PANEL;
}

std::string glimmer::DebugPanelSystem::GetName() {
    return "glimmer.DebugPanelSystem";
}
