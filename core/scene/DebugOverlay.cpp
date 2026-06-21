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

#include "AppContext.h"
#include "../Config.h"
#include "core/utils/StringUtils.h"
#include "fmt/xchar.h"


glimmer::DebugOverlay::DebugOverlay(AppContext* context)
    : Scene(context)
{
    resourcePackManager_ = appContext_->GetResourcePackManager();
    preloadColors_ = appContext_->GetPreloadColors();
    Init();
}

void glimmer::DebugOverlay::Update(const float delta)
{
    if (!displayDebugPanel_)
    {
        return;
    }
    if (delta <= 0.0F)
    {
        return;
    }
    fpsAccumTime_ += delta;
    fpsFrameCount_ += 1;
    constexpr float kFpsUpdateInterval = 1.0F;
    if (fpsAccumTime_ >= kFpsUpdateInterval)
    {
        fps_ = static_cast<float>(fpsFrameCount_) / fpsAccumTime_;
        frameTimeMs_ = fpsAccumTime_ / static_cast<float>(fpsFrameCount_) * 1000.0F;
        // Average time consumption per frame (ms) 平均每帧耗时(ms)
        fpsFrameCount_ = 0;
        fpsAccumTime_ = 0.0F;
    }
}

void glimmer::DebugOverlay::Render(SDL_Renderer* renderer)
{
    if (!displayDebugPanel_)
    {
        return;
    }

    //Draw the SDL screen coordinates
    //绘制SDL屏幕坐标
    const int labelSpacing = static_cast<int>(50 * uiScale_);
    for (int x = 0; x <= windowWidth_; x += labelSpacing)
    {
        SDL_Texture* texture = nullptr;
        auto textureIterator = numberTextureMap_.find(x);
        if (textureIterator == numberTextureMap_.end())
        {
            std::shared_ptr<SDL_Texture> texturePtr = resourcePackManager_->CreateStringTexture(
                std::to_string(x), &preloadColors_->textColor);
            numberTextureMap_[x] = texturePtr;
            texture = texturePtr.get();
        }
        else
        {
            texture = textureIterator->second.get();
        }
        SDL_FRect dst = {
            static_cast<float>(x) + 2.0f * uiScale_, 2.0f * uiScale_, static_cast<float>(texture->w) * uiScale_,
            static_cast<float>(texture->h) * uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }
    for (int y = 0; y <= windowHeight_; y += labelSpacing)
    {
        SDL_Texture* texture = nullptr;
        auto textureIterator = numberTextureMap_.find(y);
        if (textureIterator == numberTextureMap_.end())
        {
            std::shared_ptr<SDL_Texture> texturePtr = resourcePackManager_->CreateStringTexture(
                std::to_string(y), &preloadColors_->textColor);
            numberTextureMap_[y] = texturePtr;
            texture = texturePtr.get();
        }
        else
        {
            texture = textureIterator->second.get();
        }
        SDL_FRect dst = {
            2.0f * uiScale_, static_cast<float>(y) + 2.0f * uiScale_, static_cast<float>(texture->w) * uiScale_,
            static_cast<float>(texture->h) * uiScale_
        };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);
    }

    std::string fpsString = fmt::format("FPS: {} {}", fps_, frameTimeMs_);
    uint64_t fpsFingerprint = StringUtils::StringToUint64(fpsString);
    SDL_Texture* texture = nullptr;
    auto fpsIterator = fpsTextures_.find(fpsFingerprint);
    if (fpsIterator == fpsTextures_.end())
    {
        std::shared_ptr<SDL_Texture> texturePtr = resourcePackManager_->CreateStringTexture(
            fpsString, &preloadColors_->textColor);
        fpsTextures_[fpsFingerprint] = texturePtr;
        texture = texturePtr.get();
    }
    else
    {
        texture = fpsIterator->second.get();
    }
    SDL_FRect fpsRect = {
        static_cast<float>(windowWidth_) - texture->w * uiScale_,
        static_cast<float>(windowHeight_) - texture->h * uiScale_,
        texture->w * uiScale_,
        texture->h * uiScale_
    };
    SDL_RenderTexture(renderer, texture, nullptr, &fpsRect);
}

void glimmer::DebugOverlay::OnConfigChanged(const Config* config)
{
    displayDebugPanel_ = config->debug.displayDebugPanel;
    uiScale_ = config->window.uiScale;
}

void glimmer::DebugOverlay::OnWindowSizeChanged(const int width, const int height)
{
    windowWidth_ = width;
    windowHeight_ = height;
}
#endif
