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
#include "HomeScene.h"

#include <random>

#include "AppContext.h"
#include "CreateWorldScene.h"
#include "fmt/xchar.h"
#include "imgui.h"
#include "SavedGamesScene.h"
#include "core/Config.h"
#include "core/saves/SavesManager.h"

#include <chrono>

std::string glimmer::HomeScene::GetCopyrightString()
{
    constexpr int startYear = 2025;
    auto now = std::chrono::system_clock::now();
    auto days = std::chrono::floor<std::chrono::days>(now);
    std::chrono::year_month_day ymd{days};
    int currentYear = static_cast<int>(ymd.year());
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


glimmer::HomeScene::HomeScene(AppContext* context)
    : Scene(context)
{
    GetAppContext()->PlayMainMenuBGM();
    GetAppContext()->SetRandomSlogan();
    Init();
}

void glimmer::HomeScene::RenderImGui(SDL_Renderer* renderer)
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    // 获取窗口大小
    const ImVec2 windowSize(static_cast<float>(windowWidth_), static_cast<float>(windowHeight_));
    ImGui::SetNextWindowSize(windowSize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Home",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


    // 设置标题字体（需提前加载，见下方说明）
    const ImVec2 titleSize = ImGui::CalcTextSize("Glimmer");
    ImGui::SetCursorPosX((windowSize.x - titleSize.x) * 0.5f);
    ImGui::Text("Glimmer");

    // 按钮区域
    ImGui::GetIO().FontGlobalScale = uiScale_;
    float buttonWidth = 200.0F * uiScale_;
    float buttonHeight = 40.0F * uiScale_;
    float buttonSpacing = 10.0F * uiScale_;
    float totalHeight = 3 * buttonHeight + 2 * buttonSpacing;
    ImGui::SetCursorPosY((windowSize.y - totalHeight) * 0.5F);
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(GetAppContext()->GetLangsResources()->startGame.c_str(), ImVec2(buttonWidth, buttonHeight)))
    {
        if (GetAppContext()->GetSavesManager()->GetSavesListSize() == 0)
        {
            GetAppContext()->GetSceneManager()->PushScene(std::make_unique<CreateWorldScene>(GetAppContext()));
        }
        else
        {
            GetAppContext()->GetSceneManager()->PushScene(std::make_unique<SavedGamesScene>(GetAppContext()));
        }
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5F);
    if (ImGui::Button(GetAppContext()->GetLangsResources()->exitGame.c_str(), ImVec2(buttonWidth, buttonHeight)))
    {
        OnBackPressed();
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    float margin = 10.0F;
    float spacing = 4.0F;

    float lineHeight = ImGui::GetFontSize();

    // ===== 计算版本号位置 =====
    float versionPosY = windowSize.y - lineHeight - margin;
    float versionPosX =
        windowSize.x - ImGui::CalcTextSize(GAME_VERSION_STRING).x - margin;
    float currentY = versionPosY - spacing;
    for (int i = static_cast<int>(hyperlinks_.size()) - 1; i >= 0; --i)
    {
        const auto& link = hyperlinks_[i];

        currentY -= lineHeight;

        float textWidth = ImGui::CalcTextSize(link.displayText.c_str()).x;
        float posX = windowSize.x - textWidth - margin;

        ImGui::SetCursorPos(ImVec2(posX, currentY));

        if (ImGui::TextLink(link.displayText.c_str()))
        {
            SDL_OpenURL(link.link.c_str());
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        }

        currentY -= spacing;
    }

    // ===== 绘制版本号 =====
    ImGui::SetCursorPos(ImVec2(versionPosX, versionPosY));
    ImGui::Text("%s", GAME_VERSION_STRING);


    ImGui::SetCursorPos(ImVec2(margin, versionPosY));
    ImGui::Text("%s", copyright_.c_str());
    ImGui::PopStyleColor();
    ImGui::End();
}

void glimmer::HomeScene::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::HomeScene::OnWindowSizeChanged(const int& width, const int& height)
{
    windowWidth_ = width;
    windowHeight_ = height;
}

bool glimmer::HomeScene::OnBackPressed()
{
    GetAppContext()->ExitApp();
    return true;
}

glimmer::HomeScene::~HomeScene() = default;
