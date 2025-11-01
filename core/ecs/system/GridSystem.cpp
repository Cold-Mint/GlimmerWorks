//
// Created by Cold-Mint on 2025/10/29.
//

#include "GridSystem.h"
#include "../../log/LogCat.h"
#include "../component/WorldPositionComponent.h"

void glimmer::GridSystem::Update(float delta)
{
    // 网格系统主要负责渲染，更新逻辑可以为空
}

void glimmer::GridSystem::Render(SDL_Renderer* renderer)
{
    auto cameraComponent = worldContext_->GetCameraComponent();
    auto cameraPos = worldContext_->GetCameraPosition();
    auto appContext = appContext_; // 用于访问字体 TTF_Font*
    if (cameraComponent == nullptr || cameraPos == nullptr)
        return;

    // 保存当前颜色
    Uint8 currentR, currentG, currentB, currentA;
    SDL_GetRenderDrawColor(renderer, &currentR, &currentG, &currentB, &currentA);

    const int gridSize = 50;
    const int width = 100;
    const int height = 100;

    // 世界相机位置
    const float camX = cameraPos->GetPosition().x;
    const float camY = cameraPos->GetPosition().y;

    // 计算网格的起始位置（与相机对齐）
    const int startX = -static_cast<int>(fmod(camX, gridSize));
    const int startY = -static_cast<int>(fmod(camY, gridSize));

    // 屏幕绘制区域大小
    const int pixelWidth = width * gridSize;
    const int pixelHeight = height * gridSize;

    // 画网格
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    for (int x = startX; x <= pixelWidth; x += gridSize)
        SDL_RenderLine(renderer, x, startY, x, startY + pixelHeight);
    for (int y = startY; y <= pixelHeight; y += gridSize)
        SDL_RenderLine(renderer, startX, y, startX + pixelWidth, y);

    // 绘制文字
    SDL_Color textColor = {255, 255, 255, 255};

    for (int gx = 0; gx < width; ++gx)
    {
        for (int gy = 0; gy < height; ++gy)
        {
            // 世界坐标（格子中心）
            float worldX = (gx * gridSize) - camX;
            float worldY = (gy * gridSize) - camY;

            // 屏幕坐标（格子中心）
            int screenX = startX + gx * gridSize + gridSize / 2;
            int screenY = startY + gy * gridSize + gridSize / 2;

            // 格子中心文字
            char text[64];
            snprintf(text, sizeof(text), "(%d,%d)", (int)(worldX / gridSize), (int)(worldY / gridSize));

            SDL_Surface* surface = TTF_RenderText_Blended(appContext->ttfFont, text, strlen(text), textColor);
            if (!surface)
            {
                LogCat::w("TTF_RenderText_Blended failed at (%d,%d): %s", gx, gy, SDL_GetError());
                continue;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture)
            {
                LogCat::w("SDL_CreateTextureFromSurface failed at (%d,%d): %s", gx, gy, SDL_GetError());
                SDL_DestroySurface(surface);
                continue;
            }

            SDL_FRect dst = {
                static_cast<float>(screenX - surface->w / 2),
                static_cast<float>(screenY - surface->h / 2),
                static_cast<float>(surface->w),
                static_cast<float>(surface->h)
            };

            SDL_RenderTexture(renderer, texture, nullptr, &dst);

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    // 还原颜色
    SDL_SetRenderDrawColor(renderer, currentR, currentG, currentB, currentA);
}


std::string glimmer::GridSystem::GetName()
{
    return "glimmer.GridSystem";
}

bool glimmer::GridSystem::ShouldActivate()
{
    // 当有GridComponent组件时激活系统
    return worldContext_->HasComponentType(std::type_index(typeid(GridComponent)));
}
