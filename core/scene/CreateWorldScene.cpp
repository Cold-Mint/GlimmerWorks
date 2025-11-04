//
// Created by Cold-Mint on 2025/10/20.
//

#include "CreateWorldScene.h"

#include <filesystem>
#include <random>
#include <fstream>

#include "AppContext.h"
#include "HomeScene.h"
#include "imgui.h"
#include "WorldScene.h"
#include "../Constants.h"
#include "../log/LogCat.h"
#include "../saves/Saves.h"
#include "fmt/base.h"
#include "nlohmann/json.hpp"
#include "SDL3/SDL_log.h"
namespace fs = std::filesystem;

void glimmer::CreateWorldScene::CreateWorld() const
{
    std::string name = world_name;
    if (name.empty())
    {
        SDL_Log("The name of the world cannot be empty!");
        return;
    }
    const std::string seed_input = seed_str;
    int seed_value = 0;
    try
    {
        seed_value = std::stoi(seed_input);
    }
    catch (...)
    {
        std::hash<std::string> hasher;
        seed_value = static_cast<int>(hasher(seed_input));
    }
    LogCat::d("Create a world: ", name, ", seed: ", seed_value);
    Saves saves(fs::path("saved") / name);
    if (saves.Exist())
    {
        LogCat::e("The world already exists!");
        return;
    }
    MapManifest manifest;
    manifest.seed = seed_value;
    manifest.name = name;
    manifest.gameVersionName = GAME_VERSION_STRING;
    manifest.gameVersionNumber = GAME_VERSION_NUMBER;
    saves.Create(manifest);
    auto world_context = new WorldContext(appContext, seed_value, Vector2D(0, 0), &saves);
    appContext->sceneManager->ChangeScene(new WorldScene(appContext, world_context));
}

int glimmer::CreateWorldScene::RandomSeed()
{
    static std::mt19937 rng(std::random_device{}());
    return static_cast<int>(rng());
}

bool glimmer::CreateWorldScene::HandleEvent(const SDL_Event& event)
{
    return false;
}

void glimmer::CreateWorldScene::Update(float delta)
{
    ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                            ImGuiCond_Once, ImVec2(0.5f, 0.5f));

    ImGui::Begin(appContext->langs->createWorld.c_str(), nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImGui::TextUnformatted(appContext->langs->worldName.c_str());
    ImGui::InputText("##WorldName", world_name, IM_ARRAYSIZE(world_name));

    ImGui::TextUnformatted(appContext->langs->seed.c_str());
    ImGui::InputText("##Seed", seed_str, IM_ARRAYSIZE(seed_str));
    ImGui::SameLine();
    if (ImGui::Button(appContext->langs->random.c_str()))
    {
        int new_seed = RandomSeed();
        snprintf(seed_str, sizeof(seed_str), "%d", new_seed);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(appContext->langs->cancel.c_str(), ImVec2(120, 0)))
    {
        appContext->sceneManager->ChangeScene(new HomeScene(appContext));
    }
    ImGui::SameLine();
    if (ImGui::Button(appContext->langs->createWorld.c_str(), ImVec2(120, 0)))
    {
        CreateWorld();
    }

    ImGui::End();
}


void glimmer::CreateWorldScene::Render(SDL_Renderer* renderer)
{
}
