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
#include "PauseSystem.h"
#include "imgui.h"
#include "../../world/WorldContext.h"
#include "core/utils/TimeUtils.h"
#include "core/ecs/component/PauseComponent.h"




glimmer::PauseSystem::PauseSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_PAUSE);
}

uint8_t glimmer::PauseSystem::GetRenderOrder()
{
    return RENDER_ORDER_PAUSE;
}

void glimmer::PauseSystem::Render(SDL_Renderer* renderer)
{
    if (worldContext_->IsRuning())
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    // 让窗口居中
    ImVec2 windowSize(300.0f, 200.0f);
    ImVec2 windowPos(
        (io.DisplaySize.x - windowSize.x) * 0.5f,
        (io.DisplaySize.y - windowSize.y) * 0.5f
    );

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar;

    AppContext* appContext = worldContext_->GetAppContext();
    if (ImGui::Begin("PauseMenu", nullptr, flags))
    {
        // 居中标题
        const char* title = appContext->GetLangsResources()->pause.c_str();
        ImVec2 textSize = ImGui::CalcTextSize(title);
        ImGui::SetCursorPosX((windowSize.x - textSize.x) * 0.5f);
        ImGui::TextUnformatted(title);

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 按钮宽度统一
        float buttonWidth = 200.0f;
        ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);

        if (ImGui::Button(appContext->GetLangsResources()->restore.c_str(), ImVec2(buttonWidth, 0)))
        {
            worldContext_->SetRuning(true);
        }

        ImGui::Spacing();
        ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);

        if (ImGui::Button(appContext->GetLangsResources()->saveAndExit.c_str(), ImVec2(buttonWidth, 0)))
        {
            worldContext_->SaveGame();
            appContext->SetRandomSlogan();
            appContext->PlayMainMenuBGM();
            appContext->GetSceneManager()->PopScene();
        }
    }

    ImGui::End();
}

bool glimmer::PauseSystem::OnBackPressed()
{
    worldContext_->SetRuning(!worldContext_->IsRuning());
    return true;
}


bool glimmer::PauseSystem::CanRunWhilePaused() const
{
    return true;
}

glimmer::GameSystemType glimmer::PauseSystem::GetGameSystemType() const
{
    return GameSystemType::PauseSystem;
}