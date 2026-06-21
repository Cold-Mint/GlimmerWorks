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
#include "CreateWorldScene.h"

#include <filesystem>
#include <random>

#include "../Config.h"
#include "AppContext.h"
#include "HomeScene.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "WorldScene.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"
#include "core/saves/SavesManager.h"
#include "core/utils/RandomUtils.h"
#include "core/utils/StringUtils.h"
#include "core/utils/TimeUtils.h"
#include "SDL3/SDL_log.h"
namespace fs = std::filesystem;

glimmer::CreateWorldScene::CreateWorldScene(AppContext* context) : Scene(context)
{
    const int newSeed = RandomUtils::Random<int>();
    seedStr_ = std::to_string(newSeed);
    worldName_ = RandomName();
    Init();
}

void glimmer::CreateWorldScene::CreateWorld() const
{
    std::string name = worldName_;
    if (name.empty())
    {
        SDL_Log("The name of the world cannot be empty!");
        return;
    }
    const std::string seed_input = seedStr_;
    int seedValue = 0;
    try
    {
        seedValue = std::stoi(seed_input);
    }
    catch (...)
    {
        seedValue = static_cast<int>(StringUtils::StringToUint64(seed_input));
    }
    LogCat::d("Create a world: ", name, ", seed: ", seedValue);
    MapManifest manifest;
    manifest.seed = seedValue;
    manifest.name = name;
    manifest.gameVersionName = GAME_VERSION_STRING;
    manifest.gameVersionNumber = GAME_VERSION_NUMBER;
    manifest.createTime = TimeUtils::GetCurrentTimeMs();
    manifest.lastPlayedTime = manifest.createTime;
    manifest.totalPlayTime = 0;
    SavesManager* savesManager = appContext_->GetSavesManager();
    Saves* saves =
        savesManager->
        Create(appContext_->GetConfig()->runtimePath, manifest);
    if (saves == nullptr)
    {
        LogCat::e("Failed to create world");
        return;
    }
    appContext_->GetSceneManager()->
                 ReplaceScene(std::make_unique<WorldScene>(
                     appContext_, std::make_unique<WorldContext>(
                         appContext_, savesManager->GetMapManifest(savesManager->GetSavesListSize() - 1),
                         saves)));
}

void glimmer::CreateWorldScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

std::string glimmer::CreateWorldScene::RandomName() const
{
    const std::vector<std::string>& prefixList = appContext_->GetLangsResources()->worldNamePrefix;
    const std::vector<std::string>& suffixList = appContext_->GetLangsResources()->worldNameSuffix;
    if (prefixList.empty())
    {
        throw std::runtime_error("CreateWorldScene::RandomName: worldNamePrefix is empty");
    }
    if (suffixList.empty())
    {
        throw std::runtime_error("CreateWorldScene::RandomName: worldNameSuffix is empty");
    }

    const auto randomPrefixIdx = RandomUtils::Random<size_t>(0, prefixList.size() - 1);
    const auto randomSuffixIdx = RandomUtils::Random<size_t>(0, suffixList.size() - 1);
    if (appContext_->GetLanguage().compare(0, 2, "en") == 0)
    {
        return prefixList[randomPrefixIdx] + " " + suffixList[randomSuffixIdx];
    }
    return prefixList[randomPrefixIdx] + suffixList[randomSuffixIdx];
}

void glimmer::CreateWorldScene::RenderImGui(SDL_Renderer* renderer)
{
    ImGui::GetIO().FontGlobalScale = uiScale_;
    ImGui::SetNextWindowSize(ImVec2(400 * uiScale_, 250 * uiScale_), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Once, ImVec2(0.5f, 0.5f));

    LangsResources* langsResources = appContext_->GetLangsResources();
    ImGui::Begin(langsResources->createWorld.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::TextUnformatted(langsResources->worldName.c_str());
    ImGui::InputText("##WorldName", &worldName_);
    ImGui::SameLine();
    if (ImGui::Button((langsResources->random + "##randomWorldName").c_str()))
    {
        worldName_ = RandomName();
    }

    ImGui::TextUnformatted(langsResources->seed.c_str());
    ImGui::InputText("##Seed", &seedStr_);
    ImGui::SameLine();
    if (ImGui::Button((langsResources->random + "##randomSeed").c_str()))
    {
        const int newSeed = RandomUtils::Random<int>();
        seedStr_ = std::to_string(newSeed);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(langsResources->cancel.c_str(), ImVec2(120 * uiScale_, 0)))
    {
        appContext_->GetSceneManager()->PopScene();
    }
    ImGui::SameLine();
    if (ImGui::Button(langsResources->createWorld.c_str(), ImVec2(120 * uiScale_, 0)))
    {
        CreateWorld();
    }

    ImGui::End();
}
