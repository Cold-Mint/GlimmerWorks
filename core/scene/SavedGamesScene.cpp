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
#include "../Config.h"
#include "CreateWorldScene.h"
#include "WorldScene.h"
#include "../saves/SavesManager.h"
#include "../saves/MapManifest.h"
#include "imgui.h"
#include <ctime>
#include <sstream>

#include "../utils/TimeUtils.h"

glimmer::SavedGamesScene::SavedGamesScene(AppContext *context)
    : Scene(context) {
}

bool glimmer::SavedGamesScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::SavedGamesScene::Update(float delta) {
}

void glimmer::SavedGamesScene::Render(SDL_Renderer *renderer) {
    int winW, winH;
    if (!SDL_GetRenderOutputSize(renderer, &winW, &winH)) {
        return;
    }
    const float uiScale = appContext->GetConfig()->window.uiScale;
    ImGui::GetIO().FontGlobalScale = uiScale;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(winW), static_cast<float>(winH)));

    ImGui::Begin("Saved Games Scene", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    ImGui::TextUnformatted(appContext->GetLangsResources()->savedGames.c_str());
    if (ImGui::Button(appContext->GetLangsResources()->createWorld.c_str())) {
        appContext->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(appContext));
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::SameLine();
    if (ImGui::Button(appContext->GetLangsResources()->cancel.c_str())) {
        ImGui::End();
        appContext->GetSceneManager()->PopScene();
        return;
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::Separator();

    ImGui::BeginChild(appContext->GetLangsResources()->savesList.c_str(), ImVec2(0, -50 * uiScale), true);

    auto savesManager = appContext->GetSavesManager();
    size_t count = savesManager->GetSavesListSize();

    for (size_t i = 0; i < count; ++i) {
        auto manifest = savesManager->GetMapManifest(i);
        if (!manifest) continue;

        std::stringstream ss;
        time_t timeVal = manifest->lastPlayedTime / 1000;
        tm *tmInfo = localtime(&timeVal);
        char timeBuffer[64];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", tmInfo);

        ss << manifest->name << "\n";
        ss << "Version: " << manifest->gameVersionName << " | ";
        ss << "Played: " << timeBuffer << " | ";
        ss << "Seed: " << manifest->seed << " | ";
        ss << "TotalPlay: " << TimeUtils::FormatTimeMs(manifest->totalPlayTime);

        std::string label = ss.str();

        if (ImGui::Selectable(label.c_str(), selected_save_index == static_cast<int>(i), 0, ImVec2(0, 0))) {
            selected_save_index = static_cast<int>(i);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
    }
    ImGui::EndChild();

    if (selected_save_index >= 0 && selected_save_index < static_cast<int>(count)) {
        if (ImGui::Button(appContext->GetLangsResources()->loadGame.c_str())) {
            auto saves = savesManager->GetSave(selected_save_index);
            auto manifest = savesManager->GetMapManifest(selected_save_index);
            if (saves && manifest) {
                appContext->GetSceneManager()->PushScene(std::make_unique<WorldScene>(
                    appContext, std::make_unique<WorldContext>(appContext, manifest, saves)));
            }
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }
        ImGui::SameLine();
        if (ImGui::Button(appContext->GetLangsResources()->deleteGame.c_str())) {
            ImGui::OpenPopup(appContext->GetLangsResources()->deleteGame.c_str());
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        if (ImGui::BeginPopupModal(appContext->GetLangsResources()->deleteGame.c_str(), nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted(appContext->GetLangsResources()->wantDeleteThisSave.c_str());
            ImGui::Separator();

            if (ImGui::Button(appContext->GetLangsResources()->confirm.c_str(), ImVec2(120, 0))) {
                savesManager->DeleteSave(selected_save_index);
                selected_save_index = -1;
                ImGui::CloseCurrentPopup();
                if (savesManager->GetSavesListSize() == 0) {
                    ImGui::EndPopup();
                    ImGui::End();
                    appContext->GetSceneManager()->ReplaceScene(std::make_unique<CreateWorldScene>(appContext));
                    return;
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button(appContext->GetLangsResources()->cancel.c_str(), ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}
