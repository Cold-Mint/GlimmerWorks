//
// Created by Cold-Mint on 2025/12/1.
//

#include "AndroidControlSystem.h"
#include "../../scene/AppContext.h"
#include "../../Config.h"
#include "../../log/LogCat.h"
#include "../../Constants.h"

namespace glimmer {
    bool IsPointInRect(float x, float y, const SDL_FRect &r) {
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
    }

    std::string AndroidControlSystem::GetName() {
        return "AndroidControlSystem";
    }

    uint8_t AndroidControlSystem::GetRenderOrder() {
        return RENDER_ORDER_ANDROID_CTRL;
    }

    void SendKeyEvent(SDL_Keycode key, bool down) {
        SDL_Event event;
        event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
        event.key.key = key;
        event.key.down = down;
        event.key.repeat = false;
        SDL_PushEvent(&event);
    }

    bool AndroidControlSystem::HandleEvent(const SDL_Event &event) {
        switch (event.type) {
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_MOTION: {
                int windowW = 0;
                int windowH = 0;
                SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);
                float x = event.tfinger.x * static_cast<float>(windowW);
                float y = event.tfinger.y * static_cast<float>(windowH);
                SDL_FingerID fingerId = event.tfinger.fingerID;

                ButtonType newType = ButtonType::None;
                if (IsPointInRect(x, y, leftRect)) { newType = ButtonType::Left; } else if (
                    IsPointInRect(x, y, rightRect)) { newType = ButtonType::Right; } else if (
                    IsPointInRect(x, y, jumpRect)) { newType = ButtonType::Jump; }

                ButtonType currentType = ButtonType::None;
                if (activeTouches.contains(fingerId)) {
                    currentType = activeTouches[fingerId];
                }

                if (newType != currentType) {
                    if (currentType != ButtonType::None) {
                        if (currentType == ButtonType::Left) {
                            LogCat::i("Button Left Released");
                            SendKeyEvent(SDLK_A, false);
                        } else if (currentType == ButtonType::Right) {
                            LogCat::i("Button Right Released");
                            SendKeyEvent(SDLK_D, false);
                        } else if (currentType == ButtonType::Jump) {
                            LogCat::i("Button Jump Released");
                            SendKeyEvent(SDLK_SPACE, false);
                        }
                    }

                    if (newType != ButtonType::None) {
                        activeTouches[fingerId] = newType;
                        if (newType == ButtonType::Left) {
                            LogCat::i("Button Left Pressed");
                            SendKeyEvent(SDLK_A, true);
                        } else if (newType == ButtonType::Right) {
                            LogCat::i("Button Right Pressed");
                            SendKeyEvent(SDLK_D, true);
                        } else if (newType == ButtonType::Jump) {
                            LogCat::i("Button Jump Pressed");
                            SendKeyEvent(SDLK_SPACE, true);
                        }
                    } else {
                        activeTouches.erase(fingerId);
                    }
                }
                break;
            }
            case SDL_EVENT_FINGER_UP: {
                SDL_FingerID fingerId = event.tfinger.fingerID;
                if (activeTouches.contains(fingerId)) {
                    ButtonType type = activeTouches[fingerId];
                    if (type == ButtonType::Left) {
                        LogCat::i("Button Left Released");
                        SendKeyEvent(SDLK_A, false);
                    } else if (type == ButtonType::Right) {
                        LogCat::i("Button Right Released");
                        SendKeyEvent(SDLK_D, false);
                    } else if (type == ButtonType::Jump) {
                        LogCat::i("Button Jump Released");
                        SendKeyEvent(SDLK_SPACE, false);
                    }
                    activeTouches.erase(fingerId);
                }
                break;
            }
        }
        return false;
    }

    void AndroidControlSystem::Update(float delta) {
    }

    void AndroidControlSystem::Render(SDL_Renderer *renderer) {
        if (!renderer) return;
        if (!leftTexture) {
            auto load = [&](const char *path) {
                return appContext_->GetResourcePackManager()->LoadTextureFromFile(
                    appContext_, path);
            };
            leftTexture = load("gui/left.png");
            rightTexture = load("gui/right.png");
            jumpTexture = load("gui/jump.png");
            leftPressedTexture = load("gui/left_pressed.png");
            rightPressedTexture = load("gui/right_pressed.png");
            jumpPressedTexture = load("gui/jump_pressed.png");
        }

        float uiScale = appContext_->GetConfig()->window.uiScale;
        int windowW = 0;
        int windowH = 0;
        SDL_GetWindowSize(appContext_->GetWindow(), &windowW, &windowH);

        const auto winW = static_cast<float>(windowW);
        const auto winH = static_cast<float>(windowH);

        float btnSize = 180.0F * uiScale;
        float padding = 30.0F * uiScale;

        leftRect = {padding, winH - btnSize - padding, btnSize, btnSize};
        rightRect = {padding + btnSize + padding, winH - btnSize - padding, btnSize, btnSize};
        jumpRect = {winW - btnSize - padding, winH - btnSize - padding, btnSize, btnSize};

        bool leftActive = false;
        bool rightActive = false;
        bool jumpActive = false;

        for (auto const &[fingerId, type]: activeTouches) {
            if (type == ButtonType::Left) leftActive = true;
            if (type == ButtonType::Right) rightActive = true;
            if (type == ButtonType::Jump) jumpActive = true;
        }

        SDL_RenderTexture(renderer, leftActive && leftPressedTexture ? leftPressedTexture.get() : leftTexture.get(),
                          nullptr, &leftRect);
        SDL_RenderTexture(renderer, rightActive && rightPressedTexture ? rightPressedTexture.get() : rightTexture.get(),
                          nullptr, &rightRect);
        SDL_RenderTexture(renderer, jumpActive && jumpPressedTexture ? jumpPressedTexture.get() : jumpTexture.get(),
                          nullptr, &jumpRect);
#if  !defined(NDEBUG)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderRect(renderer, &leftRect);
        SDL_RenderRect(renderer, &rightRect);
        SDL_RenderRect(renderer, &jumpRect);
#endif
    appContext_->RestoreColorRenderer(renderer);
    }
}
