//
// Created by Cold-Mint on 2025/11/3.
//

#include "DebugPanelSystem.h"
#include <cmath>

#include "../../Constants.h"
#include "../component/PlayerComponent.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "box2d/box2d.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "core/utils/ColorUtils.h"
#include "core/world/Tile.h"
#include "core/world/generator/Chunk.h"
#include "fmt/xchar.h"


bool glimmer::DebugPanelSystem::ShouldActivate() {
    return worldContext_->IsRuning() && worldContext_->GetAppContext()->GetConfig()->debug.displayDebugPanel;
}

void glimmer::DebugPanelSystem::RenderDebugText(SDL_Renderer *renderer, int windowW, const std::string &text, float y,
                                                SDL_Color textColor, SDL_Color textBGColor) const {
    SDL_Surface *surface = TTF_RenderText_Blended(
        worldContext_->GetAppContext()->GetFont(), text.c_str(), text.length(), textColor
    );
    if (surface == nullptr) {
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SDL_DestroySurface(surface);
        return;
    }
    SDL_FRect dst{
        static_cast<float>(windowW - surface->w - 4),
        y,
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, textBGColor.r, textBGColor.g, textBGColor.b, textBGColor.a);
    SDL_RenderFillRect(renderer, &dst);
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


void glimmer::DebugPanelSystem::RenderCrosshairToEdge(SDL_Renderer *renderer, float screenX, float screenY) const {
    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(worldContext_->GetAppContext()->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 230, 0, 200);

    SDL_FRect hLine = {0.0f, screenY, static_cast<float>(windowW), 1.0f};
    SDL_RenderFillRect(renderer, &hLine);

    SDL_FRect vLine = {screenX, 0.0f, 1.0f, static_cast<float>(windowH)};
    SDL_RenderFillRect(renderer, &vLine);
}


void glimmer::DebugPanelSystem::RenderPlayerInfo(
    SDL_Renderer *renderer,
    const int windowW, const int windowH
) const {
    if (worldContext_ == nullptr || windowW <= 0 || windowH <= 0) {
        return;
    }
    const GameEntity::ID player = worldContext_->GetPlayerEntity();
    const auto rigidBody2dComponent = worldContext_->GetComponent<RigidBody2DComponent>(player);
    if (rigidBody2dComponent == nullptr || !rigidBody2dComponent->IsReady()) {
        return;
    }
    const b2Vec2 currentVel = b2Body_GetLinearVelocity(rigidBody2dComponent->GetBodyId());
    char speedText[128];
    snprintf(speedText, sizeof(speedText), "Player Speed: (%.1f, %.1f)", currentVel.x, currentVel.y);
    SDL_Color color = {255, 255, 180, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(
        worldContext_->GetAppContext()->GetFont(),
        speedText,
        strlen(speedText),
        color
    );
    if (!surface) return;

    const float bottomOffsetY = 20.0f;
    float textX = static_cast<float>(windowW) / 2.0f - static_cast<float>(surface->w) / 2.0f;
    float textY = static_cast<float>(windowH) - static_cast<float>(surface->h) - bottomOffsetY;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect dst = {
            textX,
            textY,
            static_cast<float>(surface->w),
            static_cast<float>(surface->h)
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}


void glimmer::DebugPanelSystem::RenderChunkBounds(SDL_Renderer *renderer, const CameraComponent *cameraComponent,
                                                  const WorldVector2D cameraPosition) {
    //Calculate the size of each block(World Coordinates)
    //计算每个区块的尺寸（世界坐标）
    auto viewportRect = cameraComponent->GetViewportRect(cameraPosition);
    float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    float halfTile = TILE_SIZE * 0.5f;
    int minChunkX = static_cast<int>(floorf(viewportRect.x / chunkWorldSize));
    int minChunkY = static_cast<int>(floorf(viewportRect.y / chunkWorldSize));
    int maxChunkX = static_cast<int>(floorf((viewportRect.x + viewportRect.w) / chunkWorldSize));
    int maxChunkY = static_cast<int>(floorf((viewportRect.y + viewportRect.h) / chunkWorldSize));
    for (int cx = minChunkX; cx <= maxChunkX; ++cx) {
        for (int cy = minChunkY; cy <= maxChunkY; ++cy) {
            WorldVector2D chunkWorldPos{
                static_cast<float>(cx) * chunkWorldSize - halfTile,
                static_cast<float>(cy) * chunkWorldSize - halfTile
            };
            CameraVector2D screenPos =
                    cameraComponent->GetViewPortPosition(
                        cameraPosition,
                        chunkWorldPos
                    );
            float pixelSize = chunkWorldSize * cameraComponent->GetZoom();
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 180);
            SDL_FRect rect{
                screenPos.x,
                screenPos.y - pixelSize,
                pixelSize,
                pixelSize
            };

            SDL_FRect outline{
                rect.x,
                rect.y,
                rect.w,
                rect.h
            };
            SDL_RenderRect(renderer, &outline);
        }
    }
}

glimmer::DebugPanelSystem::DebugPanelSystem(WorldContext *worldContext) : GameSystem(worldContext) {
}


void glimmer::DebugPanelSystem::Render(SDL_Renderer *renderer) {
    AppContext *appContext = worldContext_->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    int windowW = 0;
    int windowH = 0;
    SDL_GetWindowSize(appContext->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) {
        return;
    }
    float yOffset = 0.0F;
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraTransform = worldContext_->GetCameraTransform2D();
    bool inPointInViewport = false;
    if (cameraComponent != nullptr) {
        inPointInViewport = cameraComponent->IsPointInViewport(cameraTransform->GetPosition(), mousePosition_);
    }
    RenderChunkBounds(renderer, cameraComponent, cameraTransform->GetPosition());
    RenderPlayerInfo(renderer, windowW, windowH);

    if (!inPointInViewport) {
        //Do not display the tile debugging information that is outside the screen.
        //不要显示在屏幕之外的瓦片调试信息。
        AppContext::RestoreColorRenderer(renderer);
        return;
    }
    ChunkGenerator *chunkGenerator = worldContext_->GetChunkGenerator();
    constexpr float lineSpacing = 20.0F;
    auto tileLayers = worldContext_->GetEntityIDWithComponents<TileLayerComponent>();
    float totalLines = 1.0F + static_cast<float>(tileLayers.size());
    float totalTextHeight = totalLines * lineSpacing;
    yOffset = (static_cast<float>(windowH) - totalTextHeight) / 2.0F;
    std::string mouseText = fmt::format(
        fmt::runtime(appContext->GetLangsResources()->mousePosition),
        mousePosition_.x, mousePosition_.y
    );
    RenderDebugText(renderer, windowW, mouseText, yOffset,
                    appContext->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                    appContext->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
    yOffset += lineSpacing;
    bool firstLayer = true;
    TileVector2D tileCoord = TileLayerComponent::WorldToTile(mousePosition_);
    for (auto &tileEntity: tileLayers) {
        auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(tileEntity);
        if (tileLayer == nullptr) {
            continue;
        }

        TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileCoord);
        if (firstLayer) {
            float elevation = ChunkGenerator::GetElevation(tileCoord.y);
            std::string tileDebugInfo = fmt::format(
                fmt::runtime(appContext->GetLangsResources()->tileDebugInfo),
                tileCoord.x, tileCoord.y,
                chunkRelative.x, chunkRelative.y,
                chunkGenerator->GetHumidity(tileCoord),
                chunkGenerator->GetTemperature(tileCoord, elevation),
                chunkGenerator->GetErosion(tileCoord),
                elevation,
                chunkGenerator->GetWeirdness(tileCoord)
            );
            RenderDebugText(renderer, windowW, tileDebugInfo, yOffset,
                            appContext->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                            appContext->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
            yOffset += lineSpacing;
            firstLayer = false;
        }

        auto tile = tileLayer->GetSelfLayerTile(tileCoord);
        if (tile == nullptr) {
            continue;
        }
        std::string tileResDebugInfo = fmt::format(
            fmt::runtime(appContext->GetLangsResources()->tileResDebugInfo),
            static_cast<uint8_t>(tile->GetLayerType()), tile->GetId(), tile->GetHardness(), tile->GetName()
        );
        RenderDebugText(renderer, windowW, tileResDebugInfo, yOffset,
                        appContext->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
        yOffset += lineSpacing;
    }
    const Color *finalLightColor = worldContext_->GetFinalLightColor(tileCoord);
    if (finalLightColor == nullptr) {
        std::string totalLight = fmt::format(
            fmt::runtime(appContext->GetLangsResources()->totalLight),
            -1, -1, -1, -1
        );
        RenderDebugText(renderer, windowW, totalLight, yOffset,
                        appContext->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
    } else {
        std::string totalLight = fmt::format(
            fmt::runtime(appContext->GetLangsResources()->totalLight),
            finalLightColor->a, finalLightColor->r, finalLightColor->g, finalLightColor->b
        );
        RenderDebugText(renderer, windowW, totalLight, yOffset,
                        appContext->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
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
        int chunkIndexX = pos.x >> CHUNK_SHIFT;
        int chunkIndexY = pos.y >> CHUNK_SHIFT;

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
    SDL_FRect viewport = cameraComponent->GetViewportRect(cameraTransform->GetPosition());

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
    SDL_Surface *s = TTF_RenderText_Blended(worldContext_->GetAppContext()->GetFont(), chunkText, strlen(chunkText),
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
    CameraVector2D screenPos = cameraComponent->GetViewPortPosition(cameraTransform->GetPosition(), mousePosition_);
    RenderCrosshairToEdge(renderer, screenPos.x, screenPos.y);
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::DebugPanelSystem::HandleEvent(const SDL_Event &event) {
    if (worldContext_ == nullptr) {
        return false;
    }
    const CameraComponent *cameraComponent = worldContext_->GetCameraComponent();
    if (cameraComponent == nullptr) {
        return false;
    }
    const Transform2DComponent *transform2dComponent = worldContext_->GetCameraTransform2D();
    if (transform2dComponent == nullptr) {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        mousePosition_ = cameraComponent->GetWorldPosition(
            transform2dComponent->GetPosition(),
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
