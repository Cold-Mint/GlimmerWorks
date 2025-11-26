//
// Created by Cold-Mint on 2025/10/13.
//

#include "ConsoleOverlay.h"

#include "AppContext.h"
#include "../log/LogCat.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "fmt/color.h"

void glimmer::ConsoleOverlay::SetLastCursorPos(const int cursorPos) {
    lastCursorPos_ = cursorPos;
}

void glimmer::ConsoleOverlay::SetCommandStructure(const std::vector<std::string> &commandStructure) {
    commandStructure_ = commandStructure;
}

void glimmer::ConsoleOverlay::SetCommandStructureHighlightIndex(int commandStructureHighlightIndex) {
    commandStructureHighlightIndex_ = commandStructureHighlightIndex;
}

void glimmer::ConsoleOverlay::SetCommandSuggestions(const std::vector<std::string> &commandSuggestions) {
    commandSuggestions_ = commandSuggestions;
}

int glimmer::ConsoleOverlay::GetLastCursorPos() const {
    return lastCursorPos_;
}

bool glimmer::ConsoleOverlay::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.scancode == SDL_SCANCODE_GRAVE) {
            show_ = !show_;
            if (show_) {
                focusNextFrame_ = true;
            }
            return true;
        }
    }
    return false;
}

void glimmer::ConsoleOverlay::Update(float delta) {
}

void glimmer::ConsoleOverlay::addMessage(const std::string &message) {
    messages_.push_back(message);
}

int glimmer::ConsoleOverlay::InputCallback(const ImGuiInputTextCallbackData *data) {
    //When the text changes
    //当文本改变时
    auto *overlay = static_cast<ConsoleOverlay *>(data->UserData);
    int cursorPos = data->CursorPos;
    if (cursorPos != overlay->GetLastCursorPos()) {
        const std::string cmdStr(data->Buf, data->BufTextLen);
        overlay->SetLastCursorPos(cursorPos);

        std::string currentText(data->Buf, data->BufTextLen);
        const auto commandArgs = CommandArgs(cmdStr);
        overlay->SetCommandSuggestions(overlay->appContext->commandManager->GetSuggestions(commandArgs, cursorPos));
        overlay->SetCommandStructure(CommandManager::GetCommandStructure(commandArgs));
        overlay->SetCommandStructureHighlightIndex(commandArgs.GetTokenIndexAtCursor(cursorPos));
    }
    return 0;
}

void glimmer::ConsoleOverlay::Render(SDL_Renderer *renderer) {
    if (!show_) return;
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
    ImGui::BeginChild("Messages", ImVec2(0, windowHeight - inputHeight - 70), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(messages_.size()));
    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            ImGui::TextUnformatted(messages_[i].c_str());
        }
    }
    clipper.End();
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    //Command Suggestion Label
    //命令建议标签
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    for (int i = 0; i < commandStructure_.size(); i++) {
        if (i == commandStructureHighlightIndex_) {
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 120, 255, 255));
            ImGui::TextUnformatted(commandStructure_[i].c_str());
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
        } else {
            ImGui::TextUnformatted(commandStructure_[i].c_str());
        }
        if (i < commandStructure_.size() - 1) {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::PushItemWidth(-1);
    if (focusNextFrame_) {
        ImGui::SetKeyboardFocusHere();
        focusNextFrame_ = false;
    }
    if (ImGui::InputText("##Input", inputBuffer_.data(), inputBuffer_.size(), ImGuiInputTextFlags_EnterReturnsTrue |
                                                                              ImGuiInputTextFlags_CallbackAlways,
                         reinterpret_cast<ImGuiInputTextCallback>(&ConsoleOverlay::InputCallback), this)) {
        //It is executed when the player presses the Enter key.
        //当玩家按下Enter键后执行。
        if (inputBuffer_[0] != '\0') {
            const std::string cmdStr(inputBuffer_.data(), strnlen(inputBuffer_.data(), inputBuffer_.size()));
            addMessage("> " + cmdStr);
            CommandExecutor::ExecuteAsync(cmdStr, appContext->commandManager,
                                          [this](const CommandResult result, const std::string &cmd) {
                                              std::string message;
                                              std::string pattern;
                                              switch (result) {
                                                  case CommandResult::Success:
                                                      pattern = appContext->langs->executedSuccess;
                                                      break;
                                                  case CommandResult::Failure:
                                                      pattern = appContext->langs->executionFailed;
                                                      break;
                                                  case CommandResult::EmptyArgs:
                                                      message = appContext->langs->commandIsEmpty;
                                                      break;
                                                  case CommandResult::NotFound:
                                                      pattern = appContext->langs->commandNotFound;
                                                      break;
                                              }
                                              if (!pattern.empty()) {
                                                  message = fmt::format(fmt::runtime(pattern), cmd);
                                              }
                                              addMessage(message);
                                          },
                                          [this](const std::string &text) {
                                              addMessage(text);
                                          });
            ImGui::SetScrollHereY(1.0f);
        }
        inputBuffer_.fill('\0');
        focusNextFrame_ = true;
    }
    ImGui::PopItemWidth();
    ImGui::End();
}


glimmer::ConsoleOverlay::~ConsoleOverlay() = default;
