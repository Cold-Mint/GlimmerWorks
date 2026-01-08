//
// Created by Cold-Mint on 2026/1/8.
//

#include "PauseSystem.h"

#include "imgui.h"
#include "../../world/WorldContext.h"


uint8_t glimmer::PauseSystem::GetRenderOrder() {
    return RENDER_ORDER_PAUSE;
}

void glimmer::PauseSystem::Render(SDL_Renderer *renderer) {
    if (worldContext_->IsRuning()) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();

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

    if (ImGui::Begin("PauseMenu", nullptr, flags)) {
        // 居中标题
        const char *title = appContext_->GetLangsResources()->pause.c_str();
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

        if (ImGui::Button(appContext_->GetLangsResources()->restore.c_str(), ImVec2(buttonWidth, 0))) {
            worldContext_->SetRuning(true);
        }

        ImGui::Spacing();
        ImGui::SetCursorPosX((windowSize.x - buttonWidth) * 0.5f);

        if (ImGui::Button(appContext_->GetLangsResources()->saveAndExit.c_str(), ImVec2(buttonWidth, 0))) {
            auto mapManifestMessageData = worldContext_->GetSaves()->ReadMapManifest();
            if (!mapManifestMessageData.has_value()) {
                LogCat::e("Error get map Manifest ");
                return;
            }
            const long endTime = TimeUtils::GetCurrentTimeMs();
            mapManifestMessageData->set_totalplaytime(
                mapManifestMessageData->totalplaytime() + (endTime - worldContext_->GetStartTime()));
            mapManifestMessageData->set_lastplayedtime(endTime);
            if (!worldContext_->GetSaves()->WriteMapManifest(mapManifestMessageData.value())) {
                LogCat::e("Error update map Manifest ");
                return;
            }
            appContext_->GetSceneManager()->PopScene();
        }
    }

    ImGui::End();
}

bool glimmer::PauseSystem::OnBackPressed() {
    worldContext_->SetRuning(!worldContext_->IsRuning());
    return true;
}

bool glimmer::PauseSystem::CanRunWhilePaused() const {
    return true;
}

std::string glimmer::PauseSystem::GetName() {
    return "glimmer.PauseSystem";
}
