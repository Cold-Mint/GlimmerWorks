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

#include "MainScene.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "SDL3/SDL_timer.h"

glimmer::SplashScene::SplashScene(AppContext* context)
    : Scene(context)
{
    LogCat::i("Creating SplashScene, duration: 2000ms");
    nextSceneTime_ = SDL_GetTicks() + 2000;
    Init();
}

void glimmer::SplashScene::LoadDocuments()
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("splash/splash");
    LoadSingleDocument(&resourceRef);
}

void glimmer::SplashScene::Update(float delta)
{
    if (!sceneJumped_ && SDL_GetTicks() >= nextSceneTime_)
    {
        sceneJumped_ = true;
        AppContext* appContext = GetAppContext();
        if (appContext == nullptr)
        {
            return;
        }
        SceneManager* sceneManager = appContext->GetSceneManager();
        if (sceneManager == nullptr)
        {
            return;
        }
        appContext->GetMainThreadDispatcher()->PostToNextMainFrame([appContext, sceneManager]
        {
            sceneManager->ReplaceScene(std::make_unique<MainScene>(appContext));
        });
    }
}

glimmer::SplashScene::~SplashScene() = default;
