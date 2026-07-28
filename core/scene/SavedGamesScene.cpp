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

#include <algorithm>
#include "CreateWorldScene.h"
#include "WorldScene.h"
#include "core/Config.h"
#include "core/saves/MapManifest.h"
#include "core/saves/SavesManager.h"
#include "core/saves/Saves.h"
#include "core/world/WorldContext.h"

#include "core/context/AppContext.h"
#include "core/utils/StringUtils.h"
#include "core/utils/TimeUtils.h"
#include "core/log/LogCat.h"
#include "core/rmi/dataModel/SaveItem.h"
#include "fmt/xchar.h"
#include "RmlUi/Core/Elements/ElementFormControlInput.h"

glimmer::SavedGamesScene::SavedGamesScene(AppContext* context)
    : Scene(context),
      langsResources_(context->GetLangsResources()), savesManager_(context->GetSavesManager()),
      sceneManager_(context->GetSceneManager())
{
    if (savesManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "savesManager_ == nullptr");
        return;
    }
    if (sceneManager_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "sceneManager_ == nullptr");
        return;
    }
    UpdateSaveItems();
    Init();
}

void glimmer::SavedGamesScene::UpdateSaveItems()
{
    savedGamesDataModel_.saveItems.clear();
    if (savesManager_ == nullptr)
    {
        return;
    }
    const auto& keyword = savedGamesDataModel_.searchKeyword;
    const auto indices = savesManager_->FilterByKeyword(keyword);
    for (const size_t i : indices)
    {
        const MapManifest* manifest = savesManager_->GetMapManifest(i);
        if (manifest == nullptr)
        {
            continue;
        }
        SaveItem item;
        item.name = manifest->name;
        item.allowCheats = manifest->allowCheats;
        item.lastPlayedTime = TimeUtils::FormatTime(manifest->lastPlayedTime);
        item.index = static_cast<int>(i);
        item.selected = i == static_cast<size_t>(savedGamesDataModel_.selectedSaveIndex);
        if (keyword.empty())
        {
            item.prefix = StringUtils::MakeRawText(manifest->name);
            item.keyword.clear();
            item.suffix.clear();
        }
        else
        {
            std::string lowerName = manifest->name;
            std::ranges::transform(lowerName, lowerName.begin(),
                                   [](unsigned char c) { return std::tolower(c); });
            std::string lowerKeyword = keyword;
            std::ranges::transform(lowerKeyword, lowerKeyword.begin(),
                                   [](unsigned char c) { return std::tolower(c); });
            size_t keywordIndex = lowerName.find(lowerKeyword);
            if (keywordIndex == std::string::npos)
            {
                item.prefix = StringUtils::MakeRawText(manifest->name);
                item.keyword.clear();
                item.suffix.clear();
            }
            else
            {
                auto fullView = std::string_view(manifest->name);
                item.prefix = StringUtils::MakeRawText(fullView.substr(0, keywordIndex));
                item.suffix = StringUtils::MakeRawText(fullView.substr(keywordIndex + keyword.size()));
                item.keyword = StringUtils::MakeRawText(fullView.substr(keywordIndex, keyword.size()));
            }
        }
        savedGamesDataModel_.saveItems.push_back(item);
    }
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
        LogCat::w(std::source_location::current(), "savesManager_ == nullptr");
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
        LogCat::w(std::source_location::current(), "saves or manifest is nullptr");
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
        LogCat::w(std::source_location::current(), "savesManager_ == nullptr");
        return;
    }
    if (index < 0 || index >= static_cast<int>(savesManager_->GetSavesListSize()))
    {
        LogCat::w(std::source_location::current(), "invalid index");
        return;
    }
    if (savesManager_->DeleteSave(index))
    {
        savedGamesDataModel_.selectedSaveIndex = -1;
        UpdateSaveItems();
        handle.DirtyVariable("save_items");
    }
}

void glimmer::SavedGamesScene::OnBackClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr)
    {
        LogCat::w(std::source_location::current(), "appContext == nullptr");
        return;
    }
    MainThreadDispatcher* mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr)
    {
        LogCat::w(std::source_location::current(), "mainThreadDispatcher == nullptr");
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
        LogCat::w(std::source_location::current(), "appContext == nullptr");
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

void glimmer::SavedGamesScene::OnSearchChange(Rml::DataModelHandle handle, Rml::Event& event,
                                              const Rml::VariantList& args)
{
    if (searchInputElement_ == nullptr)
    {
        LogCat::w(std::source_location::current(), "searchInputElement_ == nullptr");
        return;
    }
    savedGamesDataModel_.searchKeyword = searchInputElement_->GetAttribute<Rml::String>("value", "");
    UpdateSaveItems();
    handle.DirtyVariable("save_items");
    handle.DirtyVariable("search_keyword");
}

void glimmer::SavedGamesScene::OnCreateDataModels()
{
    Rml::DataModelConstructor* constructor = CreateDataModel("saved_games_scene");
    if (constructor != nullptr)
    {
        if (auto saveItemStruct = constructor->RegisterStruct<SaveItem>())
        {
            saveItemStruct.RegisterMember("name", &SaveItem::name);
            saveItemStruct.RegisterMember("suffix", &SaveItem::suffix);
            saveItemStruct.RegisterMember("keyword", &SaveItem::keyword);
            saveItemStruct.RegisterMember("prefix", &SaveItem::prefix);
            saveItemStruct.RegisterMember("index", &SaveItem::index);
            saveItemStruct.RegisterMember("selected", &SaveItem::selected);
            saveItemStruct.RegisterMember("allow_cheats", &SaveItem::allowCheats);
            saveItemStruct.RegisterMember("last_played_time", &SaveItem::lastPlayedTime);
            constructor->RegisterArray<std::vector<SaveItem>>();
        }
        constructor->Bind("save_items", &savedGamesDataModel_.saveItems);
        constructor->Bind("search_keyword", &savedGamesDataModel_.searchKeyword);
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
        constructor->BindEventCallback(
            "on_search_change",
            &SavedGamesScene::OnSearchChange,
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
    Rml::ElementDocument* elementDocument = LoadSingleDocument(&resourceRef);
    Rml::Element* searchInput = elementDocument->GetElementById("search_input");
    if (searchInput == nullptr)
    {
        LogCat::e(std::source_location::current(), "searchInput== nullptr");
        return;
    }
    searchInputElement_ = rmlui_dynamic_cast<Rml::ElementFormControlInput*>(searchInput);
    if (searchInputElement_ == nullptr)
    {
        LogCat::e(std::source_location::current(), "searchInputElement== nullptr");
    }
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
