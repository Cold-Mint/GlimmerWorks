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
#include "DebugOverlay.h"

#include "../log/LogCat.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "AppContext.h"
#include "../Config.h"


glimmer::DebugOverlay::DebugOverlay(AppContext *context)
    : Scene(context) {
}

bool glimmer::DebugOverlay::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::DebugOverlay::Update(const float delta) {
    if (!appContext->GetConfig()->debug.displayDebugPanel) {
        return;
    }
    if (delta <= 0.0F) return;
    fpsAccumTime_ += delta;
    fpsFrameCount_ += 1;
    constexpr float kFpsUpdateInterval = 1.0F;
    if (fpsAccumTime_ >= kFpsUpdateInterval) {
        fps_ = static_cast<float>(fpsFrameCount_) / fpsAccumTime_;
        frameTimeMs_ = fpsAccumTime_ / static_cast<float>(fpsFrameCount_) * 1000.0F;
        // Average time consumption per frame (ms) 平均每帧耗时(ms)
        fpsFrameCount_ = 0;
        fpsAccumTime_ = 0.0F;
    }
}

void glimmer::DebugOverlay::Render(SDL_Renderer *renderer) {
    if (!appContext->GetConfig()->debug.displayDebugPanel) {
        return;
    }
    int windowWidth = appContext->GetWindowWidth();
    int windowHeight = appContext->GetWindowHeight();
    if (appContext->GetConfig()->debug.displayDebugPanel) {
        //Draw the SDL screen coordinates
        //绘制SDL屏幕坐标
        const float uiScale = appContext->GetConfig()->window.uiScale;
        const int labelSpacing = static_cast<int>(50 * uiScale);
        constexpr SDL_Color textColor = {180, 180, 255, 255};
        for (int x = 0; x <= windowWidth; x += labelSpacing) {
            char text[32];
            //skipcq: CXX-C1000
            (void) snprintf(text, sizeof(text), "x%d", x);
            SDL_Surface *surface = TTF_RenderText_Blended(appContext->GetFont(), text, strlen(text), textColor);
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
                static_cast<float>(x) + 2.0f * uiScale, 2.0f * uiScale, static_cast<float>(surface->w) * uiScale,
                static_cast<float>(surface->h) * uiScale
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);

            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
        for (int y = 0; y <= windowHeight; y += labelSpacing) {
            char text[32];
            //skipcq: CXX-C1000
            (void) snprintf(text, sizeof(text), "y%d", y);

            SDL_Surface *surface = TTF_RenderText_Blended(appContext->GetFont(), text, strlen(text), textColor);
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
                2.0f * uiScale, static_cast<float>(y) + 2.0f * uiScale, static_cast<float>(surface->w) * uiScale,
                static_cast<float>(surface->h) * uiScale
            };
            SDL_RenderTexture(renderer, texture, nullptr, &dst);
            SDL_DestroySurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
    // Draw the fps information (top) and window information (bottom)
    // 绘制 fps 信息（在上）和窗口信息（在下）
    if (appContext->GetFont() != nullptr) {
        SDL_Color color = {255, 255, 180, 255};

        // FPS text
        // FPS 文本
        char fpsText[64];
        //skipcq: CXX-C1000
        (void) snprintf(fpsText, sizeof(fpsText), "FPS: %.1f (%.2f ms)", fps_, frameTimeMs_);
        SDL_Surface *fpsSurface = TTF_RenderText_Blended(appContext->GetFont(), fpsText, strlen(fpsText), color);
        if (!fpsSurface) {
            LogCat::w("TTF_RenderText_Blended failed (fps): %s", SDL_GetError());
        } else {
            SDL_Texture *fpsTexture = SDL_CreateTextureFromSurface(renderer, fpsSurface);
            if (!fpsTexture) {
                LogCat::w("SDL_CreateTextureFromSurface failed (fps): %s", SDL_GetError());
            }

            char winText[64];
            //skipcq: CXX-C1000
            (void) snprintf(winText, sizeof(winText), "Window: %dx%d", windowWidth, windowHeight);
            SDL_Surface *winSurface = TTF_RenderText_Blended(appContext->GetFont(), winText, strlen(winText), color);
            if (!winSurface) {
                LogCat::w("TTF_RenderText_Blended failed (win): %s", SDL_GetError());
            } else {
                SDL_Texture *winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
                if (!winTexture) {
                    LogCat::w("SDL_CreateTextureFromSurface failed (win): %s", SDL_GetError());
                } else {
                    const float uiScale = appContext->GetConfig()->window.uiScale;
                    const float padding = 4.0F * uiScale;

                    SDL_FRect winRect = {
                        (static_cast<float>(windowWidth) - static_cast<float>(winSurface->w) * uiScale - padding),
                        (static_cast<float>(windowHeight) - static_cast<float>(winSurface->h) * uiScale - padding),
                        static_cast<float>(winSurface->w) * uiScale,
                        static_cast<float>(winSurface->h) * uiScale
                    };

                    SDL_FRect fpsRect = {
                        (static_cast<float>(windowWidth) - static_cast<float>(fpsSurface->w) * uiScale - padding),
                        (static_cast<float>(windowHeight) - static_cast<float>(winSurface->h) * uiScale - static_cast<float>(
                             fpsSurface->h) * uiScale -
                         padding * 2.0f),
                        static_cast<float>(fpsSurface->w) * uiScale,
                        static_cast<float>(fpsSurface->h) * uiScale
                    };

                    SDL_RenderTexture(renderer, fpsTexture, nullptr, &fpsRect);
                    SDL_RenderTexture(renderer, winTexture, nullptr, &winRect);

                    SDL_DestroyTexture(winTexture);
                }
                SDL_DestroySurface(winSurface);
            }

            if (fpsTexture) SDL_DestroyTexture(fpsTexture);
            SDL_DestroySurface(fpsSurface);
        }
    }
}
#endif