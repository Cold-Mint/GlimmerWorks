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

#include "AppContext.h"
#include "core/Config.h"
#include "CreateWorldScene.h"
#include "WorldScene.h"
#include "core/saves/SavesManager.h"
#include "core/saves/MapManifest.h"
#include "imgui.h"
#include <ctime>
#include <sstream>

#include "core/utils/TimeUtils.h"
#include "fmt/xchar.h"

glimmer::SavedGamesScene::SavedGamesScene(AppContext* context)
    : Scene(context),
      langsResources_(context->GetLangsResources())
{
    Init();
}

std::string glimmer::SavedGamesScene::FormatSaveLabel(const MapManifest* manifest,
                                                      const LangsResources* langsResources)
{
    std::stringstream ss;
    time_t timeVal = manifest->lastPlayedTime / 1000;

    std::tm tmBuf{};
    std::tm* tmInfo = nullptr;
#if defined(_WIN32)
    tmInfo = localtime_s(&tmBuf, &timeVal);
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

void glimmer::SavedGamesScene::RenderSavesList(const SavesManager* savesManager)
{
    const size_t count = savesManager->GetSavesListSize();
    for (size_t i = 0; i < count; ++i)
    {
        const auto manifest = savesManager->GetMapManifest(i);
        if (manifest == nullptr)
        {
            continue;
        }

        const std::string label = FormatSaveLabel(manifest, langsResources_);
        if (label.empty())
        {
            continue;
        }

        if (ImGui::Selectable(label.c_str(), selectedSaveIndex == static_cast<int>(i), 0, ImVec2(0, 0)))
        {
            selectedSaveIndex = static_cast<int>(i);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
    }
}

void glimmer::SavedGamesScene::RenderDeletePopup(SavesManager* savesManager)
{
    if (!ImGui::BeginPopupModal(langsResources_->deleteGame.c_str(), nullptr,
                                ImGuiWindowFlags_AlwaysAutoResize))
    {
        return;
    }
    ImGui::TextUnformatted(langsResources_->wantDeleteThisSave.c_str());
    ImGui::Separator();

    if (ImGui::Button(langsResources_->confirm.c_str(), ImVec2(120, 0)))
    {
        savesManager->DeleteSave(selectedSaveIndex);
        selectedSaveIndex = -1;
        ImGui::CloseCurrentPopup();
        if (savesManager->GetSavesListSize() == 0)
        {
            ImGui::EndPopup();
            ImGui::End();
            GetAppContext()->GetSceneManager()->ReplaceScene(std::make_unique<CreateWorldScene>(GetAppContext()));
        }
        ImGui::EndPopup();
        return;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button(langsResources_->cancel.c_str(), ImVec2(120, 0)))
    {
        ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::EndPopup();
}

void glimmer::SavedGamesScene::RenderSelectedSaveButtons(SavesManager* savesManager)
{
    if (const size_t count = savesManager->GetSavesListSize();
        selectedSaveIndex < 0 || selectedSaveIndex >= static_cast<int>(count))
    {
        return;
    }

    if (ImGui::Button(langsResources_->loadGame.c_str()))
    {
        auto saves = savesManager->GetSave(selectedSaveIndex);
        auto manifest = savesManager->GetMapManifest(selectedSaveIndex);
        if (saves != nullptr && manifest != nullptr)
        {
            GetAppContext()->GetSceneManager()->PushScene(std::make_unique<WorldScene>(
                GetAppContext(), std::make_unique<WorldContext>(GetAppContext(), manifest, saves)));
        }
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::SameLine();
    if (ImGui::Button(langsResources_->deleteGame.c_str()))
    {
        ImGui::OpenPopup(langsResources_->deleteGame.c_str());
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    RenderDeletePopup(savesManager);
}

void glimmer::SavedGamesScene::RenderImGui(SDL_Renderer* renderer)
{
    ImGui::GetIO().FontGlobalScale = uiScale_;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowWidth_), static_cast<float>(windowHeight_)));

    if (!ImGui::Begin("Saved Games Scene", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize))
    {
        return;
    }

    ImGui::TextUnformatted(langsResources_->savedGames.c_str());
    if (ImGui::Button(langsResources_->createWorld.c_str()))
    {
        GetAppContext()->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(GetAppContext()));
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::SameLine();
    if (ImGui::Button(langsResources_->cancel.c_str()))
    {
        ImGui::End();
        GetAppContext()->GetSceneManager()->PopScene();
        return;
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::Separator();

    ImGui::BeginChild(langsResources_->savesList.c_str(), ImVec2(0, -50 * uiScale_), true);
    RenderSavesList(GetAppContext()->GetSavesManager());
    ImGui::EndChild();

    RenderSelectedSaveButtons(GetAppContext()->GetSavesManager());

    ImGui::End();
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
