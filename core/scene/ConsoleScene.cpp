//
// Created by Cold-Mint on 2025/10/13.
//

#include "ConsoleScene.h"

#include "AppContext.h"
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

void Glimmer::ConsoleScene::addMessage(const std::string &message) {
    messages.push_back(message);
}

void Glimmer::ConsoleScene::Render(SDL_Renderer *renderer) {
    if (!show) return;
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();

    const ImGuiIO &io = ImGui::GetIO();
    const float windowHeight = io.DisplaySize.y;
    constexpr float inputHeight = 25.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::BeginChild("Messages", ImVec2(0, windowHeight - inputHeight - 10), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(messages.size()));
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            ImGui::TextUnformatted(messages[i].c_str());
        }
    }
    clipper.End();

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
        if (inputBuffer[0] != '\0') {
            addMessage(std::string("> ") + inputBuffer);
            appContext->commandExecutor->executeAsync(inputBuffer, appContext->commandManager,
                                                      [this](const CommandResult result, const std::string &cmd) {
                                                          switch (result) {
                                                              case CommandResult::Success:
                                                                  addMessage(cmd + " executed successfully");
                                                                  break;
                                                              case CommandResult::Failure:
                                                                  addMessage(cmd + "  execution failed");
                                                                  break;
                                                              case CommandResult::NotFound:
                                                                  addMessage("Command not found:" + cmd);
                                                                  break;
                                                              case CommandResult::EmptyArgs:
                                                                  addMessage("> Command is empty");
                                                                  break;
                                                          }
                                                      },
                                                      [this](const std::string &text) {
                                                          addMessage(text);
                                                      });
            ImGui::SetScrollHereY(1.0f);
        }
        inputBuffer[0] = '\0';
        focusNextFrame = true;
    }
    ImGui::PopItemWidth();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}


Glimmer::ConsoleScene::~ConsoleScene() = default;
