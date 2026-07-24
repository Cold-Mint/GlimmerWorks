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
#include "PauseSystem.h"
#include "core/world/WorldContext.h"
#include "core/scene/Scene.h"
#include "core/scene/SceneManager.h"
#include "core/scene/MainThreadDispatcher.h"
#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/Constants.h"


glimmer::PauseSystem::PauseSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_PAUSE);
    Init();
}

uint8_t glimmer::PauseSystem::GetRenderOrder()
{
    return RENDER_ORDER_PAUSE;
}

void glimmer::PauseSystem::TogglePause()
{
    paused_ = !paused_;
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        return;
    }
    worldContext->SetRuning(!paused_);
    Rml::ElementDocument* elementDocument = worldContext->GetDocument(PAUSE_DOCUMENT_NAME);
    if (elementDocument == nullptr)
    {
        return;
    }
    if (elementDocument->IsVisible())
    {
        elementDocument->Hide();
    }
    else
    {
        elementDocument->Show();
    }
}

bool glimmer::PauseSystem::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
    {
        TogglePause();
        return true;
    }
    return false;
}

bool glimmer::PauseSystem::OnBackPressed()
{
    TogglePause();
    return true;
}

void glimmer::PauseSystem::OnResumeButtonClick(Rml::DataModelHandle handle, Rml::Event& event,
                                               const Rml::VariantList& args)
{
    TogglePause();
}

void glimmer::PauseSystem::OnSaveAndExitButtonClick(Rml::DataModelHandle handle, Rml::Event& event,
                                                    const Rml::VariantList& args)
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext != nullptr)
    {
        worldContext->SaveGame();
    }
    paused_ = false;
    if (worldContext != nullptr)
    {
        worldContext->SetRuning(true);
    }

    const AppContext* appContext = worldContext != nullptr ? worldContext->GetAppContext() : nullptr;
    if (appContext != nullptr && appContext->GetMainThreadDispatcher() != nullptr)
    {
        MainThreadDispatcher* dispatcher = appContext->GetMainThreadDispatcher();
        SceneManager* sceneManager = appContext->GetSceneManager();
        dispatcher->PostToNextMainFrame([sceneManager]()
        {
            if (sceneManager != nullptr)
            {
                sceneManager->PopScene();
            }
        });
        LogCat::i("Save and exit triggered, scene pop deferred to next frame");
    }
    else
    {
        LogCat::w(std::source_location::current(), "Cannot defer scene pop: appContext or dispatcher is nullptr");
    }
}

bool glimmer::PauseSystem::CanRunWhilePaused() const
{
    return true;
}

glimmer::GameSystemType glimmer::PauseSystem::GetGameSystemType() const
{
    return GameSystemType::PauseSystem;
}