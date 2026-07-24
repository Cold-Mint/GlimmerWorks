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

glimmer::PauseSystem::PauseSystem(WorldContext* worldContext) : GuiGameSystem(worldContext)
{
    WatchComponent(COMPONENT_PAUSE);
    Init();
}

void glimmer::PauseSystem::TogglePause() const
{
    WorldContext* worldContext = GetWorldContext();
    if (worldContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot toggle pause system: worldContext is nullptr");
        return;
    }
    bool newValue = !worldContext->IsRuning();
    worldContext->SetRuning(newValue);
    if (elementDocument_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot toggle pause system: elementDocument_ is nullptr");
        return;
    }
    if (newValue)
    {
        elementDocument_->Hide();
    }
    else
    {
        elementDocument_->Show();
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
    if (worldContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot save pause system: worldContext is nullptr");
        return;
    }
    worldContext->SetRuning(false);
    worldContext->SaveGame();
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot save pause system: appContext is nullptr");
        return;
    }
    appContext->SetRandomSlogan();
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot save pause system: mainThreadDispatcher is nullptr");
        return;
    }
    SceneManager* sceneManager = appContext->GetSceneManager();
    if (sceneManager == nullptr)
    {
        LogCat::w(std::source_location::current(), "Cannot save pause system: sceneManager is nullptr");
        return;
    }
    mainThreadDispatcher->PostToNextMainFrame([sceneManager]
    {
        sceneManager->PopScene();
    });
}

void glimmer::PauseSystem::LoadDocuments(IDocumentRegistry* documentRegistry)
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("pages/pause/pause");
    elementDocument_ = documentRegistry->LoadSingleDocument(&resourceRef);
    elementDocument_->Hide();
}

void glimmer::PauseSystem::OnCreateDataModels(IDocumentRegistry* documentRegistry)
{
    Rml::DataModelConstructor* constructor = documentRegistry->CreateDataModel("pause_system");
    if (constructor == nullptr)
    {
        LogCat::e(std::source_location::current(), "Failed to create pause_system data model");
        return;
    }

    constructor->BindEventCallback("on_resume_button_click",
                                   [this](Rml::DataModelHandle handle, Rml::Event& event,
                                          const Rml::VariantList& args)
                                   {
                                       OnResumeButtonClick(handle, event, args);
                                   });

    constructor->BindEventCallback("on_save_and_exit_button_click",
                                   [this](Rml::DataModelHandle handle, Rml::Event& event,
                                          const Rml::VariantList& args)
                                   {
                                       OnSaveAndExitButtonClick(handle, event, args);
                                   });
}

glimmer::GameSystemType glimmer::PauseSystem::GetGameSystemType() const
{
    return GameSystemType::PauseSystem;
}