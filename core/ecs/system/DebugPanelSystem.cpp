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
    return worldContext_->IsRuning() && appContext_->GetConfig()->debug.displayDebugPanel;
}

void glimmer::DebugPanelSystem::RenderDebugText(SDL_Renderer *renderer, int windowW, const char *text, float y) const {
    SDL_Color color = {255, 255, 180, 255};
    SDL_Surface *s = TTF_RenderText_Blended(
        appContext_->GetFont(), text, strlen(text), color
    );
    if (!s) return;

    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    if (t) {
        SDL_FRect dst{
            static_cast<float>(windowW - s->w - 4),
            y,
            static_cast<float>(s->w),
            static_cast<float>(s->h)
        };
        SDL_RenderTexture(renderer, t, nullptr, &dst);
        SDL_DestroyTexture(t);
    }
    SDL_DestroySurface(s);
}

void glimmer::DebugPanelSystem::RenderCrosshairToEdge(SDL_Renderer *renderer, float screenX, float screenY) {
    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 200);

    SDL_FRect hLine = {0.0f, screenY, static_cast<float>(windowW), 1.0f};
    SDL_RenderFillRect(renderer, &hLine);

    SDL_FRect vLine = {screenX, 0.0f, 1.0f, static_cast<float>(windowH)};
    SDL_RenderFillRect(renderer, &vLine);
}


void glimmer::DebugPanelSystem::Render(SDL_Renderer *renderer) {
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) return;
    float yOffset = 0.0F;
    auto camera = worldContext_->GetCameraComponent();
    auto cameraTransform = worldContext_->GetCameraTransform2D();
    bool inPointInViewport = false;
    if (camera != nullptr) {
        inPointInViewport = camera->IsPointInViewport(cameraTransform->GetPosition(), mousePosition_);
    }
    if (!inPointInViewport) {
        //Do not display the tile debugging information that is outside the screen.
        //不要显示在屏幕之外的瓦片调试信息。
        return;
    }
    for (auto &entity: entities) {
        constexpr float lineSpacing = 20.0F;
        SDL_Color color = {255, 255, 180, 255};
        auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
        auto position = worldContext_->GetComponent<Transform2DComponent>(entity->GetID());
        if (!control || !position) continue;

        auto tileLayers = worldContext_->GetEntitiesWithComponents<TileLayerComponent, Transform2DComponent>();
        float totalLines = 1.0F + static_cast<float>(tileLayers.size());
        float totalTextHeight = totalLines * lineSpacing;
        yOffset = (static_cast<float>(windowH) - totalTextHeight) / 2.0F;

        char buffer[128];
        snprintf(buffer, sizeof(buffer), "mouse Position: (%.1f, %.1f)", mousePosition_.x, mousePosition_.y);

        SDL_Surface *surface = TTF_RenderText_Blended(appContext_->GetFont(), buffer, strlen(buffer), color);
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

            TileVector2D tileCoord = TileLayerComponent::WorldToTile(layerPos->GetPosition(), mousePosition_);
            TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileCoord);
            float elevation = WorldContext::GetElevation(tileCoord.y);
            snprintf(buffer, sizeof(buffer),
                     "Tile Coord:(%d, %d) chunk(%d,%d)  humidity:%f temperature：%f weirdness:%f erosion:%f elevation:%f",
                     tileCoord.x, tileCoord.y, chunkRelative.x, chunkRelative.y,
                     worldContext_->GetHumidity(tileCoord), worldContext_->GetTemperature(tileCoord, elevation),
                     worldContext_->GetWeirdness(tileCoord), worldContext_->GetErosion(tileCoord),
                     elevation);

            SDL_Surface *s = TTF_RenderText_Blended(appContext_->GetFont(), buffer, strlen(buffer), color);
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
            RenderDebugText(renderer, windowW, buffer, yOffset);
            yOffset += lineSpacing;
            auto tile = tileLayer->GetTile(tileCoord);
            if (tile != nullptr) {
                snprintf(
                    buffer, sizeof(buffer),
                    "  id:%s name:%s breakable:%s hardness:%f",
                    tile->id.c_str(),
                    tile->name.c_str(),
                    tile->breakable ? "true" : "false",
                    tile->hardness
                );

                RenderDebugText(renderer, windowW, buffer, yOffset);
                yOffset += lineSpacing;
            }
        }
    }

    // Draw Chunk Grid in Bottom-Left
    // 在左下角绘制区块网格
    const auto chunksPtr = *worldContext_->GetAllChunks();
    int playerTileX = static_cast<int>(std::floor(mousePosition_.x / TILE_SIZE));
    int playerTileY = static_cast<int>(std::floor(mousePosition_.y / TILE_SIZE));

    auto getChunkIndex = [](int tileCoord) {
        return static_cast<int>(std::floor(static_cast<float>(tileCoord) / CHUNK_SIZE));
    };

    int playerChunkX = getChunkIndex(playerTileX);
    int playerChunkY = getChunkIndex(playerTileY);

    float cellSize = 10.0F;
    float gridCenterX = 100.0F;
    float gridCenterY = static_cast<float>(windowH) - 100.0F;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw Loaded Chunks (Blue)
    // 绘制已加载的区块（蓝色）
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 128);

    for (const auto &[pos, chunk]: chunksPtr) {
        int chunkIndexX = pos.x / CHUNK_SIZE;
        int chunkIndexY = pos.y / CHUNK_SIZE;

        float drawX = gridCenterX + static_cast<float>(chunkIndexX - playerChunkX) * cellSize;
        float drawY = gridCenterY + static_cast<float>(playerChunkY - chunkIndexY) * cellSize;

        SDL_FRect rect = {drawX, drawY, cellSize - 1.0F, cellSize - 1.0F};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw Current Chunk (Red)
    // 绘制当前区块（红色）
    SDL_SetRenderDrawColor(renderer, 255, 69, 0, 200);
    SDL_FRect playerRect = {gridCenterX, gridCenterY, cellSize - 1.0F, cellSize - 1.0F};
    SDL_RenderFillRect(renderer, &playerRect);

    // Draw Visible Chunks (Orange)
    // 绘制可见区块（橙色）【修改Y轴计算：cy - playerChunkY → playerChunkY - cy】
    int visibleChunkCount = 0;
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
            float drawX = gridCenterX + static_cast<float>(cx - playerChunkX) * cellSize;
            float drawY = gridCenterY + static_cast<float>(playerChunkY - cy) * cellSize;

            SDL_FRect top = {drawX, drawY, cellSize, 1.0F};
            SDL_RenderFillRect(renderer, &top);
            SDL_FRect bottom = {drawX, drawY + cellSize - 1.0F, cellSize, 1.0F};
            SDL_RenderFillRect(renderer, &bottom);
            SDL_FRect left = {drawX, drawY, 1.0F, cellSize};
            SDL_RenderFillRect(renderer, &left);
            SDL_FRect right = {drawX + cellSize - 1.0F, drawY, 1.0F, cellSize};
            SDL_RenderFillRect(renderer, &right);
        }
    }

    // Draw Chunk Info Text
    // 绘制区块信息文本
    char chunkText[128];
    snprintf(chunkText, sizeof(chunkText), "Chunk: (%d, %d) | Vis: %d | Total: %zu",
             playerChunkX, playerChunkY, visibleChunkCount, chunksPtr.size());
    SDL_Surface *s = TTF_RenderText_Blended(appContext_->GetFont(), chunkText, strlen(chunkText),
                                            {255, 255, 255, 255});
    if (s) {
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, s);
        if (texture) {
            SDL_FRect dst = {
                48.0F,
                static_cast<float>(windowH) - static_cast<float>(s->h) - 8.0F,
                static_cast<float>(s->w),
                static_cast<float>(s->h)
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(s);
    }
    CameraVector2D screenPos = camera->GetViewPortPosition(cameraTransform->GetPosition(), mousePosition_);
    RenderCrosshairToEdge(renderer, screenPos.x, screenPos.y);
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::DebugPanelSystem::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mousePosition_ = worldContext_->GetCameraComponent()->GetWorldPosition(
            worldContext_->GetCameraTransform2D()->GetPosition(),
            CameraVector2D{
                event.motion.x, event.motion.y
            });
    }
    return false;
}

uint8_t glimmer::DebugPanelSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_PANEL;
}

std::string glimmer::DebugPanelSystem::GetName() {
    return "glimmer.DebugPanelSystem";
}
