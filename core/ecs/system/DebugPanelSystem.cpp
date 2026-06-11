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
#include "DebugPanelSystem.h"
#include <cmath>
#include "core/math/CoordinateTransformer.h"

#include "../../Constants.h"
#include "../component/Transform2DComponent.h"
#include "../component/TileLayerComponent.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "core/utils/ColorUtils.h"
#include "core/world/Tile.h"
#include "core/world/generator/Chunk.h"
#include "fmt/xchar.h"


void glimmer::DebugPanelSystem::RenderDebugText(SDL_Renderer* renderer, int windowW, const std::string& text, float y,
                                                SDL_Color textColor, SDL_Color textBGColor) const
{
    SDL_Surface* surface = TTF_RenderText_Blended(
        worldContext_->GetAppContext()->GetFont(), text.c_str(), text.length(), textColor
    );
    if (surface == nullptr)
    {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
    {
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


void glimmer::DebugPanelSystem::RenderCrosshairToEdge(SDL_Renderer* renderer, float screenX, float screenY) const
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 230, 0, 200);

    SDL_FRect hLine = {0.0F, screenY, static_cast<float>(worldContext_->GetAppContext()->GetWindowWidth()), 1.0F};
    SDL_RenderFillRect(renderer, &hLine);

    SDL_FRect vLine = {screenX, 0.0F, 1.0F, static_cast<float>(worldContext_->GetAppContext()->GetWindowHeight())};
    SDL_RenderFillRect(renderer, &vLine);
}


void glimmer::DebugPanelSystem::RenderChunkBounds(SDL_Renderer* renderer, const CameraComponent* cameraComponent,
                                                  const WorldVector2D& cameraPosition)
{
    //Calculate the size of each block(World Coordinates)
    //计算每个区块的尺寸（世界坐标）
    auto viewportRect = CoordinateTransformer::GetViewportRect(cameraPosition, cameraComponent->GetSize(),
                                                               cameraComponent->GetZoom());
    float chunkWorldSize = CHUNK_SIZE * TILE_SIZE;
    int minChunkX = static_cast<int>(floorf(viewportRect.x / chunkWorldSize));
    int minChunkY = static_cast<int>(floorf(viewportRect.y / chunkWorldSize));
    int maxChunkX = static_cast<int>(floorf((viewportRect.x + viewportRect.w) / chunkWorldSize));
    int maxChunkY = static_cast<int>(floorf((viewportRect.y + viewportRect.h) / chunkWorldSize));
    for (int chunkX = minChunkX; chunkX <= maxChunkX; ++chunkX)
    {
        for (int chunkY = minChunkY; chunkY <= maxChunkY; ++chunkY)
        {
            WorldVector2D chunkWorldPos{
                static_cast<float>(chunkX) * chunkWorldSize - HALF_TILE_SIZE,
                static_cast<float>(chunkY) * chunkWorldSize - HALF_TILE_SIZE
            };
            ScreenVector2D screenPos =
                CoordinateTransformer::WorldToScreen(
                    cameraPosition,
                    chunkWorldPos,
                    cameraComponent->GetSize(),
                    cameraComponent->GetZoom()
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

bool glimmer::DebugPanelSystem::CanActive() const
{
    return displayDebugPanel_;
}

void glimmer::DebugPanelSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_TILE_LAYER)
    {
        tileLayerComponents_.clear();
        auto tileLayerEntities = entityManager_->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        std::sort(tileLayerEntities.begin(), tileLayerEntities.end());
        for (GameEntityID tileLayerEntity : tileLayerEntities)
        {
            auto tileLayerComponent = entityManager_->GetComponent<TileLayerComponent>(tileLayerEntity);
            if (tileLayerComponent == nullptr)
            {
                continue;
            }
            tileLayerComponents_.emplace_back(tileLayerComponent);
        }
    }
}


glimmer::DebugPanelSystem::DebugPanelSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    WatchComponent(COMPONENT_TILE_LAYER);
    appContext_ = worldContext_->GetAppContext();
    configChangedId_ = appContext_->GetConfig()->RegisterOnConfigChanged(
        true, std::make_unique<std::function<void(const Config*)>>(
            [ this](const Config* newConfig)
            {
                displayDebugPanel_ = newConfig->debug.displayDebugPanel;
            }));
}

glimmer::DebugPanelSystem::~DebugPanelSystem()
{
    if (appContext_ != nullptr && configChangedId_ != INVALID_CONFIG_CALL_BACK)
    {
        Config* config = appContext_->GetConfig();
        if (config != nullptr)
        {
            config->UnregisterOnConfigChanged(configChangedId_);
        }
    }
}


void glimmer::DebugPanelSystem::Render(SDL_Renderer* renderer)
{
    if (cameraComponent_ == nullptr || cameraTransform2DComponent_ == nullptr || appContext_ == nullptr ||
        tileLayerComponents_.empty())
    {
        return;
    }
    float yOffset = 0.0F;
    bool inPointInViewport = cameraComponent_->IsPointInViewport(cameraTransform2DComponent_->GetPosition(),
                                                                 mousePosition_);

    RenderChunkBounds(renderer, cameraComponent_, cameraTransform2DComponent_->GetPosition());

    if (!inPointInViewport)
    {
        //Do not display the tile debugging information that is outside the screen.
        //不要显示在屏幕之外的瓦片调试信息。
        AppContext::RestoreColorRenderer(renderer);
        return;
    }
    int windowHeight = appContext_->GetWindowHeight();
    int windowWidth = appContext_->GetWindowWidth();
    ChunkGenerator* chunkGenerator = worldContext_->GetChunkGenerator();
    constexpr float lineSpacing = 20.0F;
    float totalLines = 1.0F + static_cast<float>(tileLayerComponents_.size());
    float totalTextHeight = totalLines * lineSpacing;
    yOffset = (static_cast<float>(windowHeight) - totalTextHeight) / 2.0F;
    ScreenVector2D ScreenVector2D = CoordinateTransformer::WorldToScreen(
        cameraTransform2DComponent_->GetPosition(), mousePosition_,
        cameraComponent_->GetSize(), cameraComponent_->GetZoom());
    std::string mouseText = fmt::format(
        fmt::runtime(appContext_->GetLangsResources()->mousePosition),
        mousePosition_.x, mousePosition_.y, ScreenVector2D.x, ScreenVector2D.y
    );
    RenderDebugText(renderer, windowWidth, mouseText, yOffset,
                    appContext_->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                    appContext_->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
    yOffset += lineSpacing;
    bool firstLayer = true;
    TileVector2D tileCoord = CoordinateTransformer::WorldToTile(mousePosition_);
    for (auto tileLayerComponent : tileLayerComponents_)
    {
        TileVector2D chunkRelative = Chunk::TileCoordinatesToChunkRelativeCoordinates(tileCoord);
        if (firstLayer)
        {
            float elevation = ChunkGenerator::GetElevation(tileCoord.y);
            std::string tileDebugInfo = fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->tileDebugInfo),
                tileCoord.x, tileCoord.y,
                chunkRelative.x, chunkRelative.y,
                chunkGenerator->GetHumidity(tileCoord),
                chunkGenerator->GetTemperature(tileCoord, elevation),
                chunkGenerator->GetErosion(tileCoord),
                elevation,
                chunkGenerator->GetWeirdness(tileCoord)
            );
            RenderDebugText(renderer, windowWidth, tileDebugInfo, yOffset,
                            appContext_->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                            appContext_->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
            yOffset += lineSpacing;
            firstLayer = false;
        }

        auto tile = tileLayerComponent->GetSelfLayerTile(tileCoord);
        if (tile == nullptr)
        {
            continue;
        }
        std::string tileResDebugInfo = fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->tileResDebugInfo),
            static_cast<uint8_t>(tile->GetLayerType()), tile->GetId(), tile->GetHardness(), tile->GetName()
        );
        RenderDebugText(renderer, windowWidth, tileResDebugInfo, yOffset,
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
        yOffset += lineSpacing;
    }
    const Color* finalLightColor = worldContext_->GetLightingBuffer()->GetFinalLightColor(tileCoord);
    if (finalLightColor == nullptr)
    {
        std::string totalLight = fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->totalLight),
            -1, -1, -1, -1
        );
        RenderDebugText(renderer, windowWidth, totalLight, yOffset,
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
    }
    else
    {
        std::string totalLight = fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->totalLight),
            finalLightColor->a, finalLightColor->r, finalLightColor->g, finalLightColor->b
        );
        RenderDebugText(renderer, windowWidth, totalLight, yOffset,
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextColor.ToSDLColor(),
                        appContext_->GetPreloadColors()->debugColor.debugPanelTextBGColor.ToSDLColor());
    }

    // Draw Chunk Grid in Bottom-Left
    // 在左下角绘制区块网格
    const auto chunksPtr = *worldContext_->GetAllChunks();
    int playerTileX = static_cast<int>(std::floor(mousePosition_.x / TILE_SIZE));
    int playerTileY = static_cast<int>(std::floor(mousePosition_.y / TILE_SIZE));

    auto getChunkIndex = [](int tileCoord)
    {
        return static_cast<int>(std::floor(static_cast<float>(tileCoord) / CHUNK_SIZE));
    };

    int playerChunkX = getChunkIndex(playerTileX);
    int playerChunkY = getChunkIndex(playerTileY);

    float cellSize = 10.0F;
    float gridCenterX = 100.0F;
    float gridCenterY = static_cast<float>(windowHeight) - 100.0F;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw Loaded Chunks (Blue)
    // 绘制已加载的区块（蓝色）
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 128);

    for (const auto& [pos, chunk] : chunksPtr)
    {
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
    SDL_FRect viewport = CoordinateTransformer::GetViewportRect(cameraTransform2DComponent_->GetPosition(),
                                                                cameraComponent_->GetSize(),
                                                                cameraComponent_->GetZoom());

    // Calculate visible chunk range
    // 计算可见区块范围
    int startChunkX = static_cast<int>(std::floor(viewport.x / TILE_SIZE / CHUNK_SIZE));
    int startChunkY = static_cast<int>(std::floor(viewport.y / TILE_SIZE / CHUNK_SIZE));
    int endChunkX = static_cast<int>(std::floor((viewport.x + viewport.w) / TILE_SIZE / CHUNK_SIZE));
    int endChunkY = static_cast<int>(std::floor((viewport.y + viewport.h) / TILE_SIZE / CHUNK_SIZE));

    visibleChunkCount = (endChunkX - startChunkX + 1) * (endChunkY - startChunkY + 1);

    SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); // Orange

    for (int cy = startChunkY; cy <= endChunkY; ++cy)
    {
        for (int cx = startChunkX; cx <= endChunkX; ++cx)
        {
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
    SDL_Surface* s = TTF_RenderText_Blended(worldContext_->GetAppContext()->GetFont(), chunkText, strlen(chunkText),
                                            {255, 255, 255, 255});
    if (s)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, s);
        if (texture)
        {
            SDL_FRect dst = {
                48.0F,
                static_cast<float>(windowHeight) - static_cast<float>(s->h) - 8.0F,
                static_cast<float>(s->w),
                static_cast<float>(s->h)
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(s);
    }
    glimmer::ScreenVector2D screenPos = CoordinateTransformer::WorldToScreen(cameraTransform2DComponent_->GetPosition(),
                                                                             mousePosition_,
                                                                             cameraComponent_->GetSize(),
                                                                             cameraComponent_->GetZoom());
    RenderCrosshairToEdge(renderer, screenPos.x, screenPos.y);
    AppContext::RestoreColorRenderer(renderer);
}

bool glimmer::DebugPanelSystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (cameraComponent_ == nullptr)
    {
        return false;
    }
    if (cameraTransform2DComponent_ == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
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

uint8_t glimmer::DebugPanelSystem::GetRenderOrder()
{
    return RENDER_ORDER_DEBUG_PANEL;
}

glimmer::GameSystemType glimmer::DebugPanelSystem::GetGameSystemType() const
{
    return GameSystemType::DebugPanelSystem;
}

#endif
