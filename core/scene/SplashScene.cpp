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
#include "SplashScene.h"

#include "AppContext.h"
#include "HomeScene.h"
#include "core/log/LogCat.h"
#include "tweeny.h"
#include "SDL3/SDL_timer.h"

glimmer::SplashScene::SplashScene(AppContext* context)
    : Scene(context)
{
    nextSceneTime_ = std::numeric_limits<Uint64>::max();
    Init();
}

void glimmer::SplashScene::Update(float delta)
{
    if (!animationFinished_ && fadeTween_.progress() < 1.0f)
    {
        fadeTween_.step(delta);
        alpha_ = fadeTween_.peek();
    }
    else if (!animationFinished_ && fadeTween_.progress() >= 1.0f)
    {
        animationFinished_ = true;
        nextSceneTime_ = SDL_GetTicks() + 200;

    }
    if (!sceneJumped_ && SDL_GetTicks() >= nextSceneTime_)
    {
        sceneJumped_ = true;
        AppContext* appContext = GetAppContext();
        appContext->GetSceneManager()->ReplaceScene(std::make_unique<HomeScene>(appContext));
    }
}

void glimmer::SplashScene::Render(SDL_Renderer* renderer)
{
    if (splashTextureResult_ == nullptr)
    {
        ResourceRef splashTextureRef;
        splashTextureRef.SetSelfPackageId(RESOURCE_REF_CORE);
        splashTextureRef.SetResourceKey("gui/splash");
        splashTextureRef.SetResourceType(RESOURCE_TEXTURE);
        splashTextureResult_ = GetAppContext()->GetResourceLocator()->FindTexture(
            &splashTextureRef);
        fadeTween_ = tweeny::from(0.0F).to(1.0F).during(1000);
        alpha_ = 0.0F;
        animationFinished_ = false;

    }

    SDL_Texture* rawTex = splashTextureResult_->GetResource();
    if (rawTex == nullptr)
    {
        return;
    }

    SDL_SetTextureAlphaMod(rawTex, static_cast<Uint8>(alpha_ * 255));
    if (!SDL_RenderTexture(renderer, rawTex, nullptr, nullptr))
    {

    }
}

glimmer::SplashScene::~SplashScene() = default;
