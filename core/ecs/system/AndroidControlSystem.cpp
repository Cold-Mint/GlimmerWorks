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
#ifdef __ANDROID__
#include "src/core/game_component_type.pb.h"
#include "core/scene/AppContext.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/world/WorldContext.h"
#include "AndroidControlSystem.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"
#include "core/Config.h"
#include "core/log/LogCat.h"
#include "core/Constants.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"

namespace glimmer {
    bool IsPointInRect(float x, float y, const SDL_FRect &r) {
        return x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h;
    }

    glimmer::GameSystemType glimmer::AndroidControlSystem::GetGameSystemType()
    {
        return GameSystemType::AndroidControlSystem;
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

    AndroidControlSystem::AndroidControlSystem(WorldContext *worldContext)
        : GameSystem(worldContext) {
        WatchComponent(COMPONENT_PLAYER);
        WatchComponent(COMPONENT_TRANSFORM_2D);
    }

    bool AndroidControlSystem::HandleEvent(const SDL_Event &event) {
        AppContext *appContext = worldContext_->GetAppContext();
        if (appContext == nullptr) {
            return false;
        }
        switch (event.type) {
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_MOTION: {
                int windowW = 0;
                int windowH = 0;
                SDL_GetWindowSize(appContext->GetWindow(), &windowW, &windowH);
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
        if (renderer == nullptr) {
            return;
        }
        AppContext *appContext = worldContext_->GetAppContext();
        if (appContext == nullptr) {
            return;
        }
        if (leftTexture != nullptr) {
            /*auto load = [&](const char *path) {
                return appContext->GetResourcePackManager()->LoadTextureFromFile(
                    appContext, path);
            };
            leftTexture = load("gui/left.png");
            rightTexture = load("gui/right.png");
            jumpTexture = load("gui/jump.png");
            leftPressedTexture = load("gui/left_pressed.png");
            rightPressedTexture = load("gui/right_pressed.png");
            jumpPressedTexture = load("gui/jump_pressed.png");*/
        }

        float uiScale = appContext->GetConfig()->window.uiScale;
        int windowW = 0;
        int windowH = 0;
        SDL_GetWindowSize(appContext->GetWindow(), &windowW, &windowH);

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
AppContext::RestoreColorRenderer (renderer);
    }
}
#endif
