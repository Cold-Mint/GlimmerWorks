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
#include "nlohmann/json.hpp"
#include "SDL3/SDL_log.h"
namespace fs = std::filesystem;

void Glimmer::CreateWorldScene::CreateWorld() {
    std::string name = world_name;
    if (name.empty()) {
        SDL_Log("世界名称不能为空！");
        return;
    }

    std::string seed_input = seed_str;
    long long seed_value = 0;

    try {
        seed_value = std::stoll(seed_input);
    } catch (...) {
        // 如果不是整数，取字符串 hash 值
        std::hash<std::string> hasher;
        seed_value = static_cast<long long>(hasher(seed_input));
    }

    SDL_Log("创建世界：%s, 种子：%lld", name.c_str(), seed_value);

    // 创建路径：/saved/[世界名称]/
    fs::path dir = fs::path("saved") / name;
    fs::create_directories(dir);

    // 写入 map.json
    nlohmann::json world_data = {
        {"name", name},
        {"seed", std::to_string(seed_value)}
    };

    std::ofstream file(dir / "map.json"); //Type std::ofstream is incomplete
    file << world_data.dump(4);
    file.close();

    SDL_Log("世界文件已保存到 %s", (dir / "map.json").string().c_str());
}

int Glimmer::CreateWorldScene::RandomSeed() {
    static std::mt19937 rng(std::random_device{}());
    return static_cast<int>(rng());
}

bool Glimmer::CreateWorldScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::CreateWorldScene::Update(float delta) {
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
    if (ImGui::Button(appContext->langs->random.c_str())) {
        int new_seed = RandomSeed();
        snprintf(seed_str, sizeof(seed_str), "%d", new_seed);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button(appContext->langs->cancel.c_str(), ImVec2(120, 0))) {
        appContext->sceneManager->changeScene(new HomeScene(appContext));
    }
    ImGui::SameLine();
    if (ImGui::Button(appContext->langs->createWorld.c_str(), ImVec2(120, 0))) {
        CreateWorld();
    }

    ImGui::End();
}


void Glimmer::CreateWorldScene::Render(SDL_Renderer *renderer) {
}
