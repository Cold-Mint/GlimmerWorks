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
#include "AppRenderer.h"
#include "log/LogCat.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "scene/SceneManager.h"
#include "GameUIMessage.h"
#include <cassert>

namespace glimmer
{
    AppRenderer::AppRenderer(AppContext* appContext, SDL_Renderer* renderer) :
        appContext_(appContext),
        renderer_(renderer)
    {
    }

    void AppRenderer::RenderFrame(const int windowWidth, const int windowHeight, const uint64_t frameStart,
                                  const float deltaTime) const
    {
        if (windowWidth <= 0 || windowHeight <= 0)
        {
            return;
        }
        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui::NewFrame();

        SDL_RenderClear(renderer_);
#if  defined(NDEBUG)
        RenderRelease();
#else
        RenderDebug();
#endif
        RenderUiMessage(windowHeight, frameStart, deltaTime);
        SDL_RenderPresent(renderer_);
    }

    void AppRenderer::RenderScenes() const
    {
        auto sceneManager = appContext_->GetSceneManager();
        if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
        {
            topScene->Render(renderer_);
        }
    }

    void AppRenderer::RenderImGui() const
    {
        auto sceneManager = appContext_->GetSceneManager();
        if (Scene* topScene = sceneManager->GetTopScene(); topScene != nullptr)
        {
            topScene->RenderImGui(renderer_);
        }
        const auto& overlayScenes = sceneManager->GetOverlayScenes();
        for (const auto overlay : overlayScenes)
        {
            overlay->RenderImGui(renderer_);
        }
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
    }

    void AppRenderer::RenderOverlays() const
    {
        auto sceneManager = appContext_->GetSceneManager();
        const auto& overlayScenes = sceneManager->GetOverlayScenes();
        for (const auto overlay : overlayScenes)
        {
            overlay->Render(renderer_);
        }
    }

    void AppRenderer::RenderRelease() const
    {
        RenderScenes();
        RenderImGui();
        RenderOverlays();
    }

    void AppRenderer::RenderDebug() const
    {
        SDL_Color oldColor;
        SDL_GetRenderDrawColor(renderer_, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);

        RenderScenes();

        SDL_Color newColor;
        SDL_GetRenderDrawColor(renderer_, &newColor.r, &newColor.g, &newColor.b, &newColor.a);
        if (oldColor.a != newColor.a || oldColor.r != newColor.r ||
            oldColor.g != newColor.g || oldColor.b != newColor.b)
        {

            assert(false);
        }

        RenderImGui();

        auto sceneManager = appContext_->GetSceneManager();
        const auto& overlayScenes = sceneManager->GetOverlayScenes();
        for (const auto overlay : overlayScenes)
        {
            SDL_GetRenderDrawColor(renderer_, &oldColor.r, &oldColor.g, &oldColor.b, &oldColor.a);
            overlay->Render(renderer_);
            SDL_Color overlayColor;
            SDL_GetRenderDrawColor(renderer_, &overlayColor.r, &overlayColor.g, &overlayColor.b, &overlayColor.a);
            if (oldColor.a != overlayColor.a || oldColor.r != overlayColor.r ||
                oldColor.g != overlayColor.g || oldColor.b != overlayColor.b)
            {

                assert(false);
            }
        }
    }

    void AppRenderer::RenderUiMessage(const int windowHeight, const uint64_t frameStart, const float deltaTime) const
    {
        auto& uiMessages = appContext_->GetGameUIMessages();
        if (uiMessages.empty())
        {
            return;
        }
        std::erase_if(uiMessages,
                      [frameStart](const GameUIMessage& msg)
                      {
                          return msg.GetExpireTime() <= frameStart;
                      });

        constexpr float padding = 16.0F;
        constexpr float spacing = 6.0F;

        float totalHeight = 0.0F;

        for (auto& msg : uiMessages)
        {
            auto& tween = msg.GetTween();
            tween.step(deltaTime);
            const float peekResult = tween.peek();
            msg.SetAlpha(peekResult);
            if (peekResult <= 0.01F)
            {
                continue;
            }
            const SDL_Texture* sdlTexture = msg.GetTexture();
            if (sdlTexture == nullptr)
            {
                continue;
            }
            totalHeight += static_cast<float>(sdlTexture->h) + spacing;
        }

        if (!uiMessages.empty() && totalHeight > 0.0F)
        {
            totalHeight -= spacing;
        }
        float startY = static_cast<float>(windowHeight) - totalHeight - padding;
        for (auto& msg : uiMessages)
        {
            if (msg.GetAlpha() <= 0.01F)
            {
                continue;
            }
            SDL_Texture* sdlTexture = msg.GetTexture();
            if (sdlTexture == nullptr)
            {
                continue;
            }
            SDL_SetTextureAlphaMod(sdlTexture, static_cast<Uint8>(msg.GetAlpha() * 255));
            const SDL_FRect dst = {
                padding,
                startY,
                static_cast<float>(sdlTexture->w),
                static_cast<float>(sdlTexture->h)
            };

            SDL_RenderTexture(renderer_, sdlTexture, nullptr, &dst);
            startY += static_cast<float>(sdlTexture->h) + spacing;
        }
    }
}
