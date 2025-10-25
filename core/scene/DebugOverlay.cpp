//
// Created by Cold-Mint on 2025/10/25.
//

#include "DebugOverlay.h"

#include "../log/LogCat.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "AppContext.h"


bool Glimmer::DebugOverlay::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_F1) {
            show = !show;
            return true;
        }
    }
    return false;
}

void Glimmer::DebugOverlay::Update(float delta) {
}

void Glimmer::DebugOverlay::Render(SDL_Renderer *renderer) {
    if (!show) {
        return;
    }
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(appContext->window, &w, &h);
    if (w <= 0 || h <= 0) {
        LogCat::e("Invalid window size: w=", w, ", h=", h);
        return;
    }
    if (appContext->debugScreenCoords) {
        //Draw the SDL screen coordinates
        //绘制SDL屏幕坐标

        constexpr int labelSpacing = 50;
        constexpr SDL_Color textColor = {180, 180, 255, 255};
        for (int x = 0; x <= w; x += labelSpacing) {
            char text[32];
            snprintf(text, sizeof(text), "x%d", x);
            SDL_Surface *surface = TTF_RenderText_Blended(appContext->ttfFont, text, strlen(text), textColor);
            if (!surface) {
                LogCat::w("TTF_RenderText_Blended failed at x=%d: %s", x, SDL_GetError());
                continue;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                LogCat::w("SDL_CreateTextureFromSurface failed at x=%d: %s", x, SDL_GetError());
                SDL_DestroySurface(surface);
                continue;
            }

            SDL_FRect dst = {
                static_cast<float>(x) + 2.0f, 2.0f, static_cast<float>(surface->w), static_cast<float>(surface->h)
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
        for (int y = 0; y <= h; y += labelSpacing) {
            char text[32];
            snprintf(text, sizeof(text), "y%d", y);

            SDL_Surface *surface = TTF_RenderText_Blended(appContext->ttfFont, text, strlen(text), textColor);
            if (!surface) {
                LogCat::w("TTF_RenderText_Blended failed at y=%d: %s", y, SDL_GetError());
                continue;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                LogCat::w("SDL_CreateTextureFromSurface failed at y=%d: %s", y, SDL_GetError());
                SDL_DestroySurface(surface);
                continue;
            }

            SDL_FRect dst = {
                2.0f, static_cast<float>(y) + 2.0f, static_cast<float>(surface->w), static_cast<float>(surface->h)
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}
