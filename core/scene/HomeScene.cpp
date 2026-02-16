//
// Created by Cold-Mint on 2025/10/17.
//

#include "HomeScene.h"

#include <random>

#include "AppContext.h"
#include "CreateWorldScene.h"
#include "imgui.h"
#include "SavedGamesScene.h"
#include "../Config.h"
#include "core/saves/SavesManager.h"

std::string glimmer::HomeScene::GetCopyrightString() {
    constexpr int startYear = 2025;
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;

    if (currentYear <= startYear) {
        return "Copyright (C) " + std::to_string(startYear) + " Cold-Mint";
    }
    return "Copyright (C) " +
           std::to_string(startYear) + "–" +
           std::to_string(currentYear) +
           " Cold-Mint";
}

glimmer::HomeScene::HomeScene(AppContext *context)
    : Scene(context) {
    hyperlinks = std::vector<Hyperlink>{};
    hyperlinks.push_back(Hyperlink("Github", "https://github.com/Cold-Mint/GlimmerWorks"));
    hyperlinks.push_back(Hyperlink("Discord", "https://discord.com/invite/CfppC9WHw8"));
    hyperlinks.push_back(Hyperlink("QQ Channel", "https://pd.qq.com/s/cntb09fr1?b=9"));
    copyright = GetCopyrightString();
}

bool glimmer::HomeScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void glimmer::HomeScene::Update(float delta) {
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

void glimmer::HomeScene::Render(SDL_Renderer *renderer) {
    int winW = 0;
    int winH = 0;
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
    const float uiScale = appContext->GetConfig()->window.uiScale;
    ImGui::GetIO().FontGlobalScale = uiScale;
    float buttonWidth = 200.0F * uiScale;
    float buttonHeight = 40.0F * uiScale;
    float buttonSpacing = 10.0F * uiScale;
    float totalHeight = 3 * buttonHeight + 2 * buttonSpacing;
    ImGui::SetCursorPosY((windowSize.y - totalHeight) * 0.5F);
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->GetLangsResources()->startGame.c_str(), ImVec2(buttonWidth, buttonHeight))) {
        if (appContext->GetSavesManager()->GetSavesListSize() == 0) {
            appContext->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(appContext));
        } else {
            appContext->GetSceneManager()->PushScene(std::make_unique<SavedGamesScene>(appContext));
        }
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->GetLangsResources()->mods.c_str(), ImVec2(buttonWidth, buttonHeight))) {
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->GetLangsResources()->settings.c_str(), ImVec2(buttonWidth, buttonHeight))) {
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(appContext->GetLangsResources()->exitGame.c_str(), ImVec2(buttonWidth, buttonHeight))) {
        OnBackPressed();
    }

    float margin = 10.0f;
    float spacing = 4.0f;

    float lineHeight = ImGui::GetFontSize();

    // ===== 计算版本号位置 =====
    float versionPosY = windowSize.y - lineHeight - margin;
    float versionPosX =
            windowSize.x - ImGui::CalcTextSize(GAME_VERSION_STRING).x - margin;

    // ===== 从版本号往上排列链接 =====
    float currentY = versionPosY - spacing;

    // 反向遍历（让 vector 顺序从上到下）
    for (int i = static_cast<int>(hyperlinks.size()) - 1; i >= 0; --i) {
        const auto &link = hyperlinks[i];

        currentY -= lineHeight;

        float textWidth = ImGui::CalcTextSize(link.displayText.c_str()).x;
        float posX = windowSize.x - textWidth - margin;

        ImGui::SetCursorPos(ImVec2(posX, currentY));

        ImVec4 linkColor(0.2f, 0.6f, 1.0f, 1.0f);
        ImGui::TextColored(linkColor, "%s", link.displayText.c_str());

        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        if (ImGui::IsItemClicked()) {
#ifdef _WIN32
            std::string cmd = "start " + link.link;
#elif __APPLE__
            std::string cmd = "open " + link.link;
#else
            std::string cmd = "xdg-open " + link.link;
#endif
            system(cmd.c_str());
        }

        currentY -= spacing;
    }

    // ===== 绘制版本号 =====
    ImGui::SetCursorPos(ImVec2(versionPosX, versionPosY));
    ImVec4 white(1.f, 1.f, 1.f, 1.f);
    ImGui::TextColored(white, "%s", GAME_VERSION_STRING);


    ImVec4 textColor(0.7f, 0.7f, 0.7f, 1.0f);
    ImGui::SetCursorPos(ImVec2(margin, versionPosY));
    ImGui::TextColored(textColor, "%s", copyright.c_str());
    ImGui::PopStyleColor();
    ImGui::End();
    AppContext::RestoreColorRenderer(renderer);
}

void glimmer::HomeScene::generateStars() {
    stars.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution xDist(0.0F, static_cast<float>(windowWidth - 1));
    std::uniform_real_distribution yDist(0.0F, static_cast<float>(windowHeight - 1));
    std::uniform_int_distribution colorDist(128, 255);
    std::uniform_real_distribution sizeDist(1.0F, 10.0F);
    constexpr int minStars = 50;
    constexpr int maxStars = 1000;
    constexpr float densityFactor = 0.03F;

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

bool glimmer::HomeScene::OnBackPressed() {
    appContext->ExitApp();
    return true;
}


glimmer::HomeScene::~HomeScene() = default;
