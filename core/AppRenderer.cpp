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
#include "scene/SceneManager.h"
#include "GameUIMessage.h"
#include <cassert>


glimmer::AppRenderer::AppRenderer(AppContext *appContext, SpriteRenderer *renderer) : appContext_(appContext),
    renderer_(renderer) {
}

void glimmer::AppRenderer::RenderFrame(const RmlContext *rmlContext, const int windowWidth, const int windowHeight,
                                       const uint64_t frameStart,
                                       const float deltaTime) const {
    if (windowWidth <= 0 || windowHeight <= 0 || renderer_ == nullptr) {
        return;
    }
    WindowContext *windowContext = appContext_->GetWindowContext();
    if (windowContext == nullptr) {
        return;
    }
    renderer_->BeginFrame(windowContext->GetWindow());
#if  defined(NDEBUG)
    RenderRelease();
#else
    RenderDebug();
#endif
    RenderUiMessage(windowHeight, frameStart);
    renderer_->EndFrame();
    rmlContext->RenderContext(renderer_->GetCommandBuffer(), renderer_->GetSwapchainTexture(),
                              renderer_->GetSwapchainWidth(), renderer_->GetSwapchainHeight());
    if (!appContext_->ProcessPendingScreenshot(windowContext->GetGpuContext(), renderer_)) {
        renderer_->SubmitFrame();
    }
}

void glimmer::AppRenderer::RenderUiMessage(int windowHeight, uint64_t frameStart) const {
    auto &uiMessages = appContext_->GetGameUIMessages();
    if (uiMessages.empty()) {
        return;
    }
    std::erase_if(uiMessages,
                  [frameStart](const GameUIMessage &msg) {
                      return msg.GetExpireTime() <= frameStart;
                  });

    constexpr float padding = 16.0F;
    constexpr float spacing = 6.0F;
    float totalHeight = 0.0F;
    for (auto &msg: uiMessages) {
        auto &tween = msg.GetTween();
        tween.step(1);
        const float peekResult = tween.peek();
        msg.SetAlpha(peekResult);
        if (peekResult <= 0.01F) {
            continue;
        }
        const GpuTexture *texture = msg.GetTexture();
        if (texture == nullptr) {
            continue;
        }
        totalHeight += static_cast<float>(texture->h) + spacing;
    }

    if (!uiMessages.empty() && totalHeight > 0.0F) {
        totalHeight -= spacing;
    }
    float startY = static_cast<float>(windowHeight) - totalHeight - padding;
    for (auto &msg: uiMessages) {
        if (msg.GetAlpha() <= 0.01F) {
            continue;
        }
        const GpuTexture *texture = msg.GetTexture();
        if (texture == nullptr) {
            continue;
        }
        const SDL_FRect dst = {
            padding,
            startY,
            static_cast<float>(texture->w),
            static_cast<float>(texture->h)
        };
        const auto alpha = static_cast<Uint8>(msg.GetAlpha() * 255);
        renderer_->DrawTexture(texture, nullptr, &dst, {255, 255, 255, alpha});
        startY += static_cast<float>(texture->h) + spacing;
    }
}

void glimmer::AppRenderer::RenderScenes() const {
    auto sceneManager = appContext_->GetSceneManager();
    if (Scene *topScene = sceneManager->GetTopScene(); topScene != nullptr) {
        topScene->Render(renderer_);
    }
}

void glimmer::AppRenderer::RenderOverlays() const {
    auto sceneManager = appContext_->GetSceneManager();
    const auto &overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        overlay->Render(renderer_);
    }
}

void glimmer::AppRenderer::RenderRelease() const {
    RenderScenes();
    RenderOverlays();
}

void glimmer::AppRenderer::RenderDebug() const {
    const SDL_Color oldColor = renderer_->GetDrawColor();

    RenderScenes();

    const SDL_Color newColor = renderer_->GetDrawColor();
    if (oldColor.a != newColor.a || oldColor.r != newColor.r ||
        oldColor.g != newColor.g || oldColor.b != newColor.b) {
        assert(false);
    }
    auto sceneManager = appContext_->GetSceneManager();
    const auto &overlayScenes = sceneManager->GetOverlayScenes();
    for (const auto overlay: overlayScenes) {
        const SDL_Color overlayOldColor = renderer_->GetDrawColor();
        overlay->Render(renderer_);
        const SDL_Color overlayColor = renderer_->GetDrawColor();
        if (overlayOldColor.a != overlayColor.a || overlayOldColor.r != overlayColor.r ||
            overlayOldColor.g != overlayColor.g || overlayOldColor.b != overlayColor.b) {
            assert(false);
        }
    }
}
