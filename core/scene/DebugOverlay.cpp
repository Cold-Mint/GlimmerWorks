//
// Created by Cold-Mint on 2025/10/25.
//

#include "DebugOverlay.h"

#include "../log/LogCat.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "AppContext.h"
#include "../Config.h"


bool glimmer::DebugOverlay::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_F1) {
            appContext->config_->debug.displayDebugPanel = !appContext->config_->debug.displayDebugPanel;
            return true;
        }
    }
    return false;
}

void glimmer::DebugOverlay::Update(const float delta) {
    if (!appContext->config_->debug.displayDebugPanel) {
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
    if (!appContext->config_->debug.displayDebugPanel) {
        return;
    }
    int w = 0;
    int h = 0;
    SDL_GetWindowSize(appContext->GetWindow(), &w, &h);
    if (w <= 0 || h <= 0) {
        LogCat::e("Invalid window size: w=", w, ", h=", h);
        return;
    }
#if  !defined(NDEBUG)
    if (appContext->config_->debug.displayDebugPanel) {
        //Draw the SDL screen coordinates
        //绘制SDL屏幕坐标
        const float uiScale = appContext->config_->window.uiScale;
        const int labelSpacing = static_cast<int>(50 * uiScale);
        constexpr SDL_Color textColor = {180, 180, 255, 255};
        for (int x = 0; x <= w; x += labelSpacing) {
            char text[32];
            //skipcq: CXX-C1000
            (void) snprintf(text, sizeof(text), "x%d", x);
            SDL_Surface *surface = TTF_RenderText_Blended(appContext->ttfFont_, text, strlen(text), textColor);
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
        for (int y = 0; y <= h; y += labelSpacing) {
            char text[32];
            //skipcq: CXX-C1000
            (void) snprintf(text, sizeof(text), "y%d", y);

            SDL_Surface *surface = TTF_RenderText_Blended(appContext->ttfFont_, text, strlen(text), textColor);
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
#endif
    // Draw the fps information (top) and window information (bottom)
    // 绘制 fps 信息（在上）和窗口信息（在下）
    if (appContext->ttfFont_) {
        SDL_Color color = {255, 255, 180, 255};

        // FPS text
        // FPS 文本
        char fpsText[64];
        //skipcq: CXX-C1000
        (void) snprintf(fpsText, sizeof(fpsText), "FPS: %.1f (%.2f ms)", fps_, frameTimeMs_);
        SDL_Surface *fpsSurface = TTF_RenderText_Blended(appContext->ttfFont_, fpsText, strlen(fpsText), color);
        if (!fpsSurface) {
            LogCat::w("TTF_RenderText_Blended failed (fps): %s", SDL_GetError());
        } else {
            SDL_Texture *fpsTexture = SDL_CreateTextureFromSurface(renderer, fpsSurface);
            if (!fpsTexture) {
                LogCat::w("SDL_CreateTextureFromSurface failed (fps): %s", SDL_GetError());
            }

            char winText[64];
            //skipcq: CXX-C1000
            (void) snprintf(winText, sizeof(winText), "Window: %dx%d", w, h);
            SDL_Surface *winSurface = TTF_RenderText_Blended(appContext->ttfFont_, winText, strlen(winText), color);
            if (!winSurface) {
                LogCat::w("TTF_RenderText_Blended failed (win): %s", SDL_GetError());
            } else {
                SDL_Texture *winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
                if (!winTexture) {
                    LogCat::w("SDL_CreateTextureFromSurface failed (win): %s", SDL_GetError());
                } else {
                    const float uiScale = appContext->config_->window.uiScale;
                    const float padding = 4.0F * uiScale;

                    SDL_FRect winRect = {
                        (static_cast<float>(w) - static_cast<float>(winSurface->w) * uiScale - padding),
                        (static_cast<float>(h) - static_cast<float>(winSurface->h) * uiScale - padding),
                        static_cast<float>(winSurface->w) * uiScale,
                        static_cast<float>(winSurface->h) * uiScale
                    };

                    SDL_FRect fpsRect = {
                        (static_cast<float>(w) - static_cast<float>(fpsSurface->w) * uiScale - padding),
                        (static_cast<float>(h) - static_cast<float>(winSurface->h) * uiScale - static_cast<float>(
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
