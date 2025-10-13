//
// Created by coldmint on 2025/10/13.
//

#include "ConsoleScene.h"

#include "../log/LogCat.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

bool Glimmer::ConsoleScene::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_GRAVE) {
            show = !show;
            if (show) {
                focusNextFrame = true;
            }
            return true;
        }
    }
    return false;
}

void Glimmer::ConsoleScene::Update(float delta) {
}

void Glimmer::ConsoleScene::Render(SDL_Renderer *renderer) {
    if (!show) return;
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();

    const ImGuiIO &io = ImGui::GetIO();
    float windowHeight = io.DisplaySize.y;
    float inputHeight = 25.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::BeginChild("Messages", ImVec2(0, windowHeight - inputHeight - 10), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto &msg: messages) {
        ImGui::TextUnformatted(msg.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::PushItemWidth(-1);
    if (focusNextFrame) {
        ImGui::SetKeyboardFocusHere();
        focusNextFrame = false;
    }
    if (ImGui::InputText("##Input", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        messages.push_back(std::string("> ") + inputBuffer);
        inputBuffer[0] = '\0';
        ImGui::SetScrollHereY(1.0f);
        focusNextFrame = true;
    }
    ImGui::PopItemWidth();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}


Glimmer::ConsoleScene::~ConsoleScene() = default;
