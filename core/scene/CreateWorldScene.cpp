//
// Created by Cold-Mint on 2025/10/20.
//

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
#include "core/utils/TimeUtils.h"
#include "SDL3/SDL_log.h"
namespace fs = std::filesystem;

glimmer::CreateWorldScene::CreateWorldScene(AppContext *context) : Scene(context) {
    const int newSeed = RandomSeed();
    seedStr_ = std::to_string(newSeed);
    worldName_ = RandomName();
}

void glimmer::CreateWorldScene::CreateWorld() const {
    std::string name = worldName_;
    if (name.empty()) {
        SDL_Log("The name of the world cannot be empty!");
        return;
    }
    const std::string seed_input = seedStr_;
    int seed_value = 0;
    try {
        seed_value = std::stoi(seed_input);
    } catch (...) {
        std::hash<std::string> hasher;
        seed_value = static_cast<int>(hasher(seed_input));
    }
    LogCat::d("Create a world: ", name, ", seed: ", seed_value);
    MapManifest manifest;
    manifest.seed = seed_value;
    manifest.name = name;
    manifest.gameVersionName = GAME_VERSION_STRING;
    manifest.gameVersionNumber = GAME_VERSION_NUMBER;
    manifest.createTime = TimeUtils::GetCurrentTimeMs();
    manifest.lastPlayedTime = manifest.createTime;
    manifest.totalPlayTime = 0;
    Saves *saves = appContext->GetSavesManager()->Create(manifest);
    if (saves == nullptr) {
        LogCat::e("Failed to create world");
        return;
    }
    appContext->GetSceneManager()->
            ReplaceScene(std::make_unique<WorldScene>(
                appContext, std::make_unique<WorldContext>(appContext, seed_value, saves)));
}

int glimmer::CreateWorldScene::RandomSeed() {
    static std::mt19937 rng(std::random_device{}());
    return static_cast<int>(rng());
}

std::string glimmer::CreateWorldScene::RandomName() const {
    const std::vector<std::string> &prefixList = appContext->GetLangsResources()->worldNamePrefix;
    const std::vector<std::string> &suffixList = appContext->GetLangsResources()->worldNameSuffix;
    if (prefixList.empty()) {
        throw std::runtime_error("CreateWorldScene::RandomName: worldNamePrefix is empty");
    }
    if (suffixList.empty()) {
        throw std::runtime_error("CreateWorldScene::RandomName: worldNameSuffix is empty");
    }
    std::mt19937 randomEngine(std::random_device{}());
    std::uniform_int_distribution<size_t> prefixDist(0, prefixList.size() - 1);
    const size_t randomPrefixIdx = prefixDist(randomEngine);
    std::uniform_int_distribution<size_t> suffixDist(0, suffixList.size() - 1);
    const size_t randomSuffixIdx = suffixDist(randomEngine);
    if (appContext->GetLanguage().compare(0, 2, "en") == 0) {
        return prefixList[randomPrefixIdx] + " " + suffixList[randomSuffixIdx];
    }
    return prefixList[randomPrefixIdx] + suffixList[randomSuffixIdx];
}

bool glimmer::CreateWorldScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::CreateWorldScene::Update(float delta) {
    const float uiScale = appContext->GetConfig()->window.uiScale;
    ImGui::GetIO().FontGlobalScale = uiScale;
    ImGui::SetNextWindowSize(ImVec2(400 * uiScale, 250 * uiScale), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Once, ImVec2(0.5f, 0.5f));

    LangsResources *langsResources = appContext->GetLangsResources();
    ImGui::Begin(langsResources->createWorld.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::TextUnformatted(langsResources->worldName.c_str());
    ImGui::InputText("##WorldName", &worldName_);
    ImGui::SameLine();
    if (ImGui::Button((langsResources->random + "##randomWorldName").c_str())) {
        worldName_ = RandomName();
    }

    ImGui::TextUnformatted(langsResources->seed.c_str());
    ImGui::InputText("##Seed", &seedStr_);
    ImGui::SameLine();
    if (ImGui::Button((langsResources->random + "##randomSeed").c_str())) {
        const int newSeed = RandomSeed();
        seedStr_ = std::to_string(newSeed);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(langsResources->cancel.c_str(), ImVec2(120 * uiScale, 0))) {
        appContext->GetSceneManager()->PopScene();
    }
    ImGui::SameLine();
    if (ImGui::Button(langsResources->createWorld.c_str(), ImVec2(120 * uiScale, 0))) {
        CreateWorld();
    }

    ImGui::End();
}


void glimmer::CreateWorldScene::Render(SDL_Renderer *renderer) {
}
