//
// Created by Cold-Mint on 2025/11/3.
//

#include "DebugPanelSystem.h"

#include "../component/PlayerControlComponent.h"
#include "../component/WorldPositionComponent.h"
#include "../component/TileLayerComponent.h"


bool glimmer::DebugPanelSystem::ShouldActivate()
{
    return appContext_->isDebugMode();
}

void glimmer::DebugPanelSystem::Render(SDL_Renderer* renderer)
{
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    int windowW = 0, windowH = 0;
    SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);
    if (windowW <= 0 || windowH <= 0) return;

    float yOffset = 0.0f;

    for (auto& entity : entities)
    {
        constexpr float lineSpacing = 20.0f;
        SDL_Color color = {255, 255, 180, 255};
        auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
        auto position = worldContext_->GetComponent<WorldPositionComponent>(entity->GetID());
        if (!control || !position) continue;

        const WorldVector2D playerPos = position->GetPosition();

        // 先统计总行数（玩家 + 所有 TileLayer）
        auto tileLayers = worldContext_->GetEntitiesWithComponents<TileLayerComponent, WorldPositionComponent>();
        float totalLines = 1.0F + static_cast<float>(tileLayers.size());
        float totalTextHeight = totalLines * lineSpacing;

        // 起始 y 坐标垂直居中
        yOffset = (static_cast<float>(windowH) - totalTextHeight) / 2.0f;

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Player World: (%.1f, %.1f)", playerPos.x, playerPos.y);

        SDL_Surface* surface = TTF_RenderText_Blended(appContext_->ttfFont, buffer, strlen(buffer), color);
        if (surface)
        {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture)
            {
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
        for (auto& tileEntity : tileLayers)
        {
            auto tileLayer = worldContext_->GetComponent<TileLayerComponent>(tileEntity->GetID());
            auto layerPos = worldContext_->GetComponent<WorldPositionComponent>(tileEntity->GetID());
            if (!tileLayer || !layerPos) continue;

            TileVector2D tileCoord = tileLayer->WorldToTile(layerPos->GetPosition(), playerPos);
            snprintf(buffer, sizeof(buffer), "Tile Coord: (%d, %d)", tileCoord.x, tileCoord.y);

            SDL_Surface* s = TTF_RenderText_Blended(appContext_->ttfFont, buffer, strlen(buffer), color);
            if (s)
            {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                if (t)
                {
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
}

uint8_t glimmer::DebugPanelSystem::GetRenderOrder()
{
    return RENDER_ORDER_DEBUG_PANEL;
}

std::string glimmer::DebugPanelSystem::GetName()
{
    return "glimmer.DebugPanelSystem";
}
