//
// Created by Cold-Mint on 2025/10/17.
//

#include "HomeScene.h"

#include <random>

#include "AppContext.h"
#include "imgui.h"
#include "../log/LogCat.h"


bool Glimmer::HomeScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::HomeScene::Update(float delta) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution dist(-3, 3);

    for (auto &star: stars) {
        const int deltaTemp = dist(gen);
        star.r = std::clamp(static_cast<int>(star.r) + deltaTemp, 128, 255);
        star.g = std::clamp(static_cast<int>(star.g) + deltaTemp, 128, 255);
        star.b = std::clamp(static_cast<int>(star.b) + deltaTemp, 128, 255);
    }
}

void Glimmer::HomeScene::Render(SDL_Renderer *renderer) {
    int winW = 0, winH = 0;
    if (!SDL_GetRenderOutputSize(renderer, &winW, &winH)) {
        return;
    }
    if (windowWidth != winW || windowHeight != winH) {
        windowWidth = winW;
        windowHeight = winH;
        generateStars();
    }

    for (const auto &star: stars) {
        SDL_SetRenderDrawColor(renderer, star.r, star.g, star.b, 255);
        SDL_FRect rect{star.x, star.y, star.size, star.size};
        SDL_RenderFillRect(renderer, &rect);
    }
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Home",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    // 获取窗口大小
    const ImVec2 windowSize(static_cast<float>(winW), static_cast<float>(winH));

    // 设置标题字体（需提前加载，见下方说明）
    const ImVec2 titleSize = ImGui::CalcTextSize("Glimmer");
    ImGui::SetCursorPosX((windowSize.x - titleSize.x) * 0.5f);
    ImGui::Text("Glimmer");

    // 按钮区域
    float buttonWidth = 200.0F;
    float buttonHeight = 40.0F;
    float buttonSpacing = 10.0F;
    float totalHeight = 3 * buttonHeight + 2 * buttonSpacing;
    ImGui::SetCursorPosY((windowSize.y - totalHeight) * 0.5F);
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->langs->startGame.c_str(), ImVec2(buttonWidth, buttonHeight))) {
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->langs->mods.c_str(), ImVec2(buttonWidth, buttonHeight))) {
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->langs->settings.c_str(), ImVec2(buttonWidth, buttonHeight))) {
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->langs->exitGame.c_str(), ImVec2(buttonWidth, buttonHeight))) {
        appContext->isRunning = false;
    }

    ImVec4 white(1.0F, 1.0F, 1.0F, 1.0F);
    ImGui::SetCursorPosY(windowSize.y - ImGui::GetFontSize() - 10.0F);
    ImGui::SetCursorPosX(windowSize.x - ImGui::CalcTextSize(GAME_VERSION_STRING).x - 10.0F);
    ImGui::TextColored(white, "%s", GAME_VERSION_STRING);
    ImGui::PopStyleColor();
    ImGui::End();
}

void Glimmer::HomeScene::generateStars() {
    stars.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution xDist(0.0F, static_cast<float>(windowWidth - 1));
    std::uniform_real_distribution yDist(0.0F, static_cast<float>(windowHeight - 1));
    std::uniform_int_distribution colorDist(128, 255);
    std::uniform_real_distribution sizeDist(1.0F, 10.0F);
    constexpr int minStars = 50;
    constexpr int maxStars = 1000;
    constexpr float densityFactor = 0.03f;

    float areaSqrt = std::sqrt(static_cast<float>(windowWidth * windowHeight));
    int numStars = static_cast<int>(areaSqrt * densityFactor);
    numStars = std::clamp(numStars, minStars, maxStars);
    numStars = std::clamp(numStars, minStars, maxStars);

    for (int i = 0; i < numStars; i++) {
        stars.push_back({
            xDist(gen),
            yDist(gen),
            static_cast<Uint8>(colorDist(gen)),
            static_cast<Uint8>(colorDist(gen)),
            static_cast<Uint8>(colorDist(gen)), sizeDist(gen)
        });
    }
}


Glimmer::HomeScene::~HomeScene() = default;
