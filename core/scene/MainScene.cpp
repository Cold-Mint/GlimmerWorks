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
#include "MainScene.h"

#include <random>

#include "CreateWorldScene.h"
#include "SavedGamesScene.h"
#include "fmt/xchar.h"
#include "core/Config.h"

#include <chrono>
#include <SDL3/SDL.h>

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "core/saves/SavesManager.h"
#include "RmlUi/Core/DataModelHandle.h"

glimmer::MainScene::MainScene(AppContext* context)
    : Scene(context)
{
    context->PlayMainMenuBGM();
    context->SetRandomSlogan();
    virtualFileSystem_ = context->GetVirtualFileSystem();
    const ResourceLocator* resourceLocator = context->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        LogCat::e(std::source_location::current(), "resourceLocator == nullptr");
        return;
    }
    nextBackgroundResourceRef_.SetSelfPackageId(RESOURCE_REF_CORE);
    nextBackgroundResourceRef_.SetResourceType(RESOURCE_TEXTURE);
    nextBackgroundResourceRef_.SetResourceKey(fmt::format("{}{}", "main_menu_bg_frames/bg_",
                                                          std::to_string(backgroundIndex_)));
    const std::shared_ptr<TextureResourceResult> textureResourceResult = resourceLocator->FindTextureRaw(
        &nextBackgroundResourceRef_);
    if (textureResourceResult == nullptr)
    {
        LogCat::e(std::source_location::current(), "textureResourceResult == nullptr");
        return;
    }
    textureFolder_ = textureResourceResult->GetTexturePath().parent_path();
    LogCat::i("old textureFolder ", textureFolder_.string());
    Init();
}

void glimmer::MainScene::LoadDocuments()
{
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("main/main");
    LoadSingleDocument(&resourceRef);
}

void glimmer::MainScene::Update(const float delta)
{
    if (backgroundTargetSecond_ == -1)
    {
        NextBackgroundFrame();
        return;
    }
    backgroundAnimTimer_ += delta;
    if (backgroundAnimTimer_ >= backgroundTargetSecond_)
    {
        NextBackgroundFrame();
        backgroundAnimTimer_ -= backgroundTargetSecond_;
    }
}

void glimmer::MainScene::OnCreateDataModels()
{
    Rml::DataModelConstructor* constructor = CreateDataModel("main_scene");
    if (constructor != nullptr)
    {
        constructor->Bind("copyright", &mainSceneDataModel_.copyright);
        constructor->Bind("game_version", &mainSceneDataModel_.gameVersion);
        if (auto linkStruct = constructor->RegisterStruct<Hyperlink>())
        {
            linkStruct.RegisterMember("text", &Hyperlink::text);
            linkStruct.RegisterMember("url", &Hyperlink::url);
            constructor->RegisterArray<std::vector<Hyperlink>>();
        }
        constructor->Bind("footer_links", &mainSceneDataModel_.hyperlinks);
        constructor->Bind("main_menu_background", &mainSceneDataModel_.mainMenuBackground);
        constructor->BindEventCallback(
            "on_start_game_click",
            &MainScene::OnStartGameClick,
            this
        );
        constructor->BindEventCallback(
            "on_exit_game_click",
            &MainScene::OnExitGameClick,
            this
        );
        constructor->BindEventCallback(
            "on_link_click",
            &MainScene::OnLinkClick,
            this
        );
        mainModelHandle_ = constructor->GetModelHandle();
    }
}

void glimmer::MainScene::OnStartGameClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    AppContext* context = GetAppContext();
    if (context == nullptr)
    {
        LogCat::w(std::source_location::current(), "context == nullptr");
        return;
    }
    SavesManager* savesManager = context->GetSavesManager();
    if (savesManager == nullptr)
    {
        LogCat::w(std::source_location::current(), "savesManager == nullptr");
        return;
    }
    if (savesManager->GetSavesListSize() > 0)
    {
        context->GetMainThreadDispatcher()->PostToNextMainFrame([ context]
        {
            context->GetSceneManager()->PushScene(std::make_unique<SavedGamesScene>(context));
        });
    }
    else
    {
        context->GetMainThreadDispatcher()->PostToNextMainFrame([ context]
        {
            context->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(context));
        });
    }
}

void glimmer::MainScene::OnExitGameClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    const AppContext* context = GetAppContext();
    if (context == nullptr)
    {
        LogCat::w(std::source_location::current(), "context == nullptr");
        return;
    }
    context->ExitApp();
}

void glimmer::MainScene::OnLinkClick(Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& args)
{
    if (args.empty())
    {
        return;
    }
    const std::string& url = args[0].Get<Rml::String>();
    SDL_OpenURL(url.c_str());
}

void glimmer::MainScene::NextBackgroundFrame()
{
    int nextIndex = backgroundIndex_ + 1;
    bool exists = false;
    for (auto& supportedTextureFormat : supportedTextureFormats_)
    {
        std::filesystem::path path = textureFolder_ / fmt::format("{}{}", "bg_",
                                                                  std::to_string(nextIndex));
        path.replace_extension(supportedTextureFormat);
        exists = virtualFileSystem_->Exists(path);
        if (exists)
        {
            break;
        }
    }
    if (exists)
    {
        SetBackgroundIndex(nextIndex);
    }
    else
    {
        SetBackgroundIndex(0);
    }
}

void glimmer::MainScene::SetBackgroundIndex(const int index)
{
    backgroundIndex_ = index;
    mainSceneDataModel_.mainMenuBackground = StringUtils::MakeTextureUrl(fmt::format(
        "{}{}", "@core:main_menu_bg_frames/bg_",
        std::to_string(backgroundIndex_)));
    mainModelHandle_.DirtyVariable("main_menu_background");
}

void glimmer::MainScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
    if (config->mainMenuBackground.targetFps <= 0.0F)
    {
        //Set to -1 for frame-by-frame refresh.
        //设置为-1每帧刷新。
        backgroundTargetSecond_ = -1;
    }
    else
    {
        backgroundTargetSecond_ = 1.0F / config->mainMenuBackground.targetFps;
    }
    supportedTextureFormats_ = config->mods.supportedTextureFormats;
}

void glimmer::MainScene::OnWindowSizeChanged(const int& width, const int& height)
{
    windowWidth_ = width;
    windowHeight_ = height;
}

bool glimmer::MainScene::OnBackPressed()
{
    GetAppContext()->ExitApp();
    return true;
}

glimmer::MainScene::~MainScene() = default;
