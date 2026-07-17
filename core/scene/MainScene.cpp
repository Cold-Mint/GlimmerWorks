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
#include "fmt/xchar.h"
#include "core/Config.h"

#include <chrono>

#include "core/context/AppContext.h"
#include "core/log/LogCat.h"
#include "RmlUi/Core/DataModelHandle.h"

std::string glimmer::MainScene::GetCopyrightString()
{
    constexpr int startYear = 2025;
    const auto now = std::chrono::system_clock::now();
    const auto days = std::chrono::floor<std::chrono::days>(now);
    std::chrono::year_month_day ymd{days};
    auto currentYear = static_cast<int>(ymd.year());
    if (!ymd.year().ok() || currentYear < 1970)
    {
        return fmt::format("Copyright (C) {} Cold-Mint", startYear);
    }
    if (currentYear <= startYear)
    {
        return fmt::format("Copyright (C) {} Cold-Mint", startYear);
    }
    return fmt::format("Copyright (C) {}–{} Cold-Mint", startYear, currentYear);
}


glimmer::MainScene::MainScene(AppContext* context)
    : Scene(context)
{
    context->PlayMainMenuBGM();
    context->SetRandomSlogan();
    Init();
    Rml::DataModelConstructor* constructor = CreateDataModel("main_scene");
    if (constructor != nullptr)
    {
        constructor->Bind("copyright", &copyright_);
        auto linkStruct = constructor->RegisterStruct<Hyperlink>();
        linkStruct.RegisterMember("text", &Hyperlink::text);
        linkStruct.RegisterMember("url", &Hyperlink::url);
        constructor->RegisterArray<std::vector<Hyperlink>>();
        constructor->Bind("footer_links", &hyperlinks_);
    }
    ResourceRef resourceRef;
    resourceRef.SetSelfPackageId(RESOURCE_REF_CORE);
    resourceRef.SetResourceType(RESOURCE_RML_PATH);
    resourceRef.SetResourceKey("main/main");
    Rml::ElementDocument* document = LoadDocument(&resourceRef);
    if (document == nullptr)
    {
        LogCat::e(std::source_location::current(), "document == nullptr");
        return;
    }
    Rml::Element* startGameElement = document->GetElementById("start_game");
    if (startGameElement != nullptr)
    {
        startGameElement->AddEventListener(Rml::EventId::Click, this);
    }

    Rml::Element* exitGameElement = document->GetElementById("exit_game");
    if (exitGameElement != nullptr)
    {
        exitGameElement->AddEventListener(Rml::EventId::Click, this);
    }
}

void glimmer::MainScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
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

void glimmer::MainScene::ProcessEvent(Rml::Event& event)
{
    const Rml::Element* element = event.GetTargetElement();
    if (element == nullptr)
    {
        return;
    }
    if (element->GetId() == "start_game")
    {
    }
    if (element->GetId() == "exit_game")
    {
        const AppContext* context = GetAppContext();
        if (context == nullptr)
        {
            LogCat::e(std::source_location::current(), "context == nullptr");
            return;
        }
        context->ExitApp();
    }
}

glimmer::MainScene::~MainScene() = default;
