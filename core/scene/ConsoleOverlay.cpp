//
// Created by Cold-Mint on 2025/10/13.
//

#include "ConsoleOverlay.h"

#include "AppContext.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "fmt/color.h"

bool glimmer::ConsoleOverlay::HandleEvent(const SDL_Event &event) {
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

void glimmer::ConsoleOverlay::Update(float delta) {
}

void glimmer::ConsoleOverlay::addMessage(const std::string &message) {
    messages.push_back(message);
}

void glimmer::ConsoleOverlay::Render(SDL_Renderer *renderer) {
    if (!show) return;
    const ImGuiIO &io = ImGui::GetIO();
    const float windowHeight = io.DisplaySize.y;
    constexpr float inputHeight = 25.0F;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushFont(ImGui::GetFont());
    ImGui::TextUnformatted(appContext->langs->console.c_str());
    ImGui::PopFont();
    ImGui::Separator();
    ImGui::BeginChild("Messages", ImVec2(0, windowHeight - inputHeight - 50), false,
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
    if (ImGui::InputText("##Input", inputBuffer.data(), inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (inputBuffer[0] != '\0') {
            const std::string cmdStr(inputBuffer.data(), strnlen(inputBuffer.data(), inputBuffer.size()));
            addMessage("> " + cmdStr);
            appContext->commandExecutor->ExecuteAsync(cmdStr, appContext->commandManager,
                                                      [this](const CommandResult result, const std::string &cmd) {
                                                          std::string message;
                                                          switch (result) {
                                                              case CommandResult::Success:
                                                                  message = fmt::format(
                                                                      fmt::runtime(appContext->langs->executedSuccess),
                                                                      cmd);
                                                                  break;
                                                              case CommandResult::Failure:
                                                                  message = fmt::format(
                                                                      fmt::runtime(appContext->langs->executionFailed),
                                                                      cmd);
                                                                  break;
                                                              case CommandResult::EmptyArgs:
                                                                  message = appContext->langs->commandIsEmpty;
                                                                  break;
                                                              case CommandResult::NotFound:
                                                                  message = fmt::format(
                                                                      fmt::runtime(appContext->langs->commandNotFound),
                                                                      cmd);
                                                                  break;
                                                          }
                                                          addMessage(message);
                                                      },
                                                      [this](const std::string &text) {
                                                          addMessage(text);
                                                      });
            ImGui::SetScrollHereY(1.0f);
        }
        inputBuffer.fill('\0');
        focusNextFrame = true;
    }
    ImGui::PopItemWidth();
    ImGui::End();
}


glimmer::ConsoleOverlay::~ConsoleOverlay() = default;
