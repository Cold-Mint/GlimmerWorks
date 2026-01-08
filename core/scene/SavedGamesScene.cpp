//
// Created by Cold-Mint on 2025/10/19.
//

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
        ss << "Seed: " << manifest->seed;

        std::string label = ss.str();

        if (ImGui::Selectable(label.c_str(), selected_save_index == static_cast<int>(i), 0, ImVec2(0, 0))) {
            selected_save_index = static_cast<int>(i);
        }
    }
    ImGui::EndChild();

    if (selected_save_index >= 0 && selected_save_index < static_cast<int>(count)) {
        if (ImGui::Button(appContext->GetLangsResources()->loadGame.c_str())) {
            auto saves = savesManager->GetSave(selected_save_index);
            auto manifest = savesManager->GetMapManifest(selected_save_index);
            if (saves && manifest) {
                appContext->GetSceneManager()->PushScene(std::make_unique<WorldScene>(
                    appContext, std::make_unique<WorldContext>(appContext, manifest->seed, saves)));
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(appContext->GetLangsResources()->deleteGame.c_str())) {
            ImGui::OpenPopup(appContext->GetLangsResources()->deleteGame.c_str());
        }

        if (ImGui::BeginPopupModal(appContext->GetLangsResources()->deleteGame.c_str(), nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted(appContext->GetLangsResources()->wantDeleteThisSave.c_str());
            ImGui::Separator();

            if (ImGui::Button(appContext->GetLangsResources()->confirm.c_str(), ImVec2(120, 0))) {
                savesManager->DeleteSave(selected_save_index);
                selected_save_index = -1;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button(appContext->GetLangsResources()->cancel.c_str(), ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}
