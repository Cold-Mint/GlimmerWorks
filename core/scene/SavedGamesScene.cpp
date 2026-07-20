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
#include "SavedGamesScene.h"

#include "CreateWorldScene.h"
#include "MainScene.h"
#include "WorldScene.h"
#include "core/Config.h"
#include "core/saves/MapManifest.h"
#include "core/saves/SavesManager.h"
#include "core/saves/Saves.h"
#include "core/world/WorldContext.h"
#include <sstream>

#include "core/context/AppContext.h"
#include "core/utils/TimeUtils.h"
#include "core/log/LogCat.h"
#include "fmt/xchar.h"

glimmer::SavedGamesScene::SavedGamesScene(AppContext* context)
    : Scene(context),
      langsResources_(context->GetLangsResources()), savesManager_(context->GetSavesManager()),
      sceneManager_(context->GetSceneManager())
{
    if (savesManager_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "savesManager_ == nullptr");
        return;
    }
    if (sceneManager_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "sceneManager_ == nullptr");
        return;
    }
    UpdateSaveItems();
    Init();
}

void glimmer::SavedGamesScene::UpdateSaveItems()
{
    saveItems_.clear();
    if (savesManager_ == nullptr)
    {
        return;
    }
    const size_t saveCount = savesManager_->GetSavesListSize();
    for (size_t i = 0; i < saveCount; ++i)
    {
        const MapManifest* manifest = savesManager_->GetMapManifest(i);
        if (manifest == nullptr)
        {
            continue;
        }
        SaveItem item;
        item.label = FormatSaveLabel(manifest, langsResources_);
        item.index = static_cast<int>(i);
        item.selected = i == static_cast<size_t>(selectedSaveIndex_);
        saveItems_.push_back(item);
    }
}

std::string glimmer::SavedGamesScene::FormatSaveLabel(const MapManifest* manifest,
                                                      const LangsResources* langsResources)
{
    std::stringstream ss;
    time_t timeVal = manifest->lastPlayedTime / 1000;

    std::tm tmBuf{};
    std::tm* tmInfo = nullptr;
#if defined(_WIN32)
    if (localtime_s(&tmBuf, &timeVal) == 0)
    {
        tmInfo = &tmBuf;
    }
#else
    tmInfo = localtime_r(&timeVal, &tmBuf);
#endif
    if (tmInfo == nullptr)
    {
        return "";
    }

    ss << manifest->name << "\n";
    ss << fmt::format(fmt::runtime(langsResources->savesDescription),
                      manifest->gameVersionName,
                      *tmInfo,
                      TimeUtils::FormatTimeMs(langsResources, manifest->totalPlayTime));

    return ss.str();
}

void glimmer::SavedGamesScene::OnSaveClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    if (args.empty())
    {
        LogCat::w(std::source_location::current(), "args.empty()");
        return;
    }
    int index = args[0].Get<int>();
    if (savesManager_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "savesManager_ == nullptr");
        return;
    }
    if (index < 0 || index >= static_cast<int>(savesManager_->GetSavesListSize()))
    {
        LogCat::w(std::source_location::current(), "invalid index");
        return;
    }
    Saves* saves = savesManager_->GetSave(index);
    MapManifest* manifest = savesManager_->GetMapManifest(index);
    if (saves == nullptr || manifest == nullptr)
    {
        LogCat::e(std::source_location::current(), "saves or manifest is nullptr");
        return;
    }
    GetAppContext()->GetSceneManager()->ReplaceScene(std::make_unique<WorldScene>(
        GetAppContext(), std::make_unique<WorldContext>(GetAppContext(), manifest, saves)));
}

void glimmer::SavedGamesScene::OnDeleteClick(Rml::DataModelHandle handle, Rml::Event& event,
                                             const Rml::VariantList& args)
{
    if (args.empty())
    {
        LogCat::w(std::source_location::current(), "args.empty()");
        return;
    }
    int index = args[0].Get<int>();
    if (savesManager_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "savesManager_ == nullptr");
        return;
    }
    if (index < 0 || index >= static_cast<int>(savesManager_->GetSavesListSize()))
    {
        LogCat::w(std::source_location::current(), "invalid index");
        return;
    }
    if (savesManager_->DeleteSave(index))
    {
        selectedSaveIndex_ = -1;
        UpdateSaveItems();
        handle.DirtyVariable("save_items");
    }
}

void glimmer::SavedGamesScene::OnBackClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext == nullptr");
        return;
    }
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        LogCat::e(std::source_location::current(), "mainThreadDispatcher == nullptr");
        return;
    }
    mainThreadDispatcher->PostToNextMainFrame([this]
    {
        sceneManager_->PopScene();
    });
}

void glimmer::SavedGamesScene::OnNewGameClick(Rml::DataModelHandle handle, Rml::Event& event,
                                              const Rml::VariantList& args)
{
    AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::e(std::source_location::current(), "appContext == nullptr");
        return;
    }
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        LogCat::w(std::source_location::current(), "mainThreadDispatcher == nullptr");
        return;
    }
    mainThreadDispatcher->PostToNextMainFrame([this, appContext]
    {
        sceneManager_->PushScene(std::make_unique<CreateWorldScene>(appContext));
    });
}

void glimmer::SavedGamesScene::OnCreateDataModels()
{
    Rml::DataModelConstructor* constructor = CreateDataModel("saved_games_scene");
    if (constructor != nullptr)
    {
        if (auto saveItemStruct = constructor->RegisterStruct<SaveItem>())
        {
            saveItemStruct.RegisterMember("label", &SaveItem::label);
            saveItemStruct.RegisterMember("index", &SaveItem::index);
            saveItemStruct.RegisterMember("selected", &SaveItem::selected);
            constructor->RegisterArray<std::vector<SaveItem>>();
        }
        constructor->Bind("save_items", &saveItems_);
        constructor->BindEventCallback(
            "on_save_click",
            &SavedGamesScene::OnSaveClick,
            this
        );
        constructor->BindEventCallback(
            "on_delete_click",
            &SavedGamesScene::OnDeleteClick,
            this
        );
        constructor->BindEventCallback(
            "on_back_click",
            &SavedGamesScene::OnBackClick,
            this
        );
        constructor->BindEventCallback(
            "on_new_game_click",
            &SavedGamesScene::OnNewGameClick,
            this
        );
    }
}

void glimmer::SavedGamesScene::LoadDocuments()
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("saves/saves");
    LoadSingleDocument(&resourceRef);
}

void glimmer::SavedGamesScene::OnWindowSizeChanged(const int& width, const int& height)
{
    windowWidth_ = width;
    windowHeight_ = height;
}


void glimmer::SavedGamesScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}
