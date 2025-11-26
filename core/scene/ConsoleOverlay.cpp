//
// Created by Cold-Mint on 2025/10/13.
//

#include "ConsoleOverlay.h"

#include <algorithm>
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

void glimmer::ConsoleOverlay::SetKeyword(std::string &keyword) {
    keyword_ = keyword;
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
        auto keyword = commandArgs.GetKeywordAtCursor(cursorPos);
        overlay->SetKeyword(keyword);
        overlay->SetCommandSuggestions(overlay->appContext->commandManager->GetSuggestions(commandArgs, cursorPos));
        overlay->SetCommandStructure(CommandManager::GetCommandStructure(commandArgs));
        overlay->SetCommandStructureHighlightIndex(commandArgs.GetTokenIndexAtCursor(cursorPos));
    }
    return 0;
}

void glimmer::ConsoleOverlay::ClikAutoCompleteItem(const std::string &suggestion) {
    const int cursorPos = lastCursorPos_;
    const std::string currentText(inputBuffer_.data(), strnlen(inputBuffer_.data(), inputBuffer_.size()));
    // Find beforeCursor
    int leftSpacePos = -1;
    for (int i = cursorPos - 1; i >= 0; --i) {
        if (currentText[i] == ' ') {
            leftSpacePos = i;
            break;
        }
    }

    std::string beforeCursor;
    if (leftSpacePos >= 0) {
        beforeCursor = currentText.substr(0, leftSpacePos + 1);
    }

    // Find afterCursor
    int rightSpacePos = -1;
    for (int i = cursorPos; i < static_cast<int>(currentText.size()); ++i) {
        if (currentText[i] == ' ') {
            rightSpacePos = i;
            break;
        }
    }

    std::string afterCursor;
    if (rightSpacePos >= 0) {
        afterCursor = currentText.substr(rightSpacePos);
    }

    // Construct newText efficiently
    std::string newText;
    newText.reserve(beforeCursor.size() + suggestion.size() + afterCursor.size());
    newText.append(beforeCursor);
    newText.append(suggestion);
    newText.append(afterCursor);

    // Update input buffer
    if (newText.length() < inputBuffer_.size()) {
        std::ranges::fill(inputBuffer_, '\0');
        std::ranges::copy(newText, inputBuffer_.begin());
        lastCursorPos_ = static_cast<int>(beforeCursor.size()) + static_cast<int>(suggestion.length());
    }

    // Clear suggestions
    commandSuggestions_.clear();
    focusNextFrame_ = true;
}

void glimmer::ConsoleOverlay::Render(SDL_Renderer *renderer) {
    if (!show_) return;
    const ImGuiIO &io = ImGui::GetIO();
    const float windowHeight = io.DisplaySize.y;
    constexpr float inputHeight = 25.0F;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    // Apply dark console theme
    // 应用深色控制台主题
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 240)); // Black background with slight transparency
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(60, 60, 60, 255)); // Dark gray border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); // White text
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(10, 10, 10, 255)); // Slightly lighter black for child windows
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(30, 30, 30, 255)); // Dark gray for input background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(40, 40, 40, 255)); // Lighter on hover
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(50, 50, 50, 255)); // Even lighter when active
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(20, 20, 20, 255)); // Dark scrollbar background
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(80, 80, 80, 255)); // Gray scrollbar grab
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(100, 100, 100, 255)); // Lighter on hover
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(120, 120, 120, 255)); // Even lighter when active
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(60, 60, 60, 255)); // Dark gray for text selection
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0F); // Window border
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0F); // Child window border

    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Console title with cyan color
    // 控制台标题使用青色
    ImGui::PushFont(ImGui::GetFont());
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255)); // Cyan for title
    ImGui::TextUnformatted(appContext->langs->console.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Separator();

    // Calculate autocomplete suggestions height dynamically
    // 动态计算自动完成建议的高度
    float suggestionsHeight = 0.0F;
    if (!commandSuggestions_.empty()) {
        // Calculate height based on number of suggestions
        // 根据建议数量计算高度
        const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        const float desiredHeight = lineHeight * static_cast<float>(commandSuggestions_.size());
        const float maxSuggestionsHeight = windowHeight * 0.4F; // 40% of screen height

        // Add padding for borders, frame padding, and scrollbar
        // 为边框、框架内边距和滚动条添加额外空间
        const float extraPadding = ImGui::GetStyle().FramePadding.y * 2 +
                                   ImGui::GetStyle().ItemSpacing.y * 2 +
                                   ImGui::GetStyle().ScrollbarSize;

        suggestionsHeight = std::min(desiredHeight + extraPadding, maxSuggestionsHeight);
    }

    // Adjust Messages child window height to account for suggestions
    // 调整消息子窗口高度以考虑建议
    const float messagesHeight = windowHeight - inputHeight - 70 - suggestionsHeight;

    ImGui::BeginChild("Messages", ImVec2(0, messagesHeight), true, // true = show border
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
    //Autocomplete Suggestions
    //自动完成建议
    if (!commandSuggestions_.empty()) {
        // Enable vertical scrollbar when content overflows
        // 当内容溢出时启用垂直滚动条
        ImGui::BeginChild("AutocompleteSuggestions", ImVec2(0, suggestionsHeight), true, // true = show border
                          ImGuiWindowFlags_AlwaysVerticalScrollbar);

        // Style buttons to look like plain text
        // 将按钮样式设置为类似普通文本
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0)); // Transparent background
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(40, 40, 40, 150)); // Dark gray on hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(60, 60, 60, 200)); // Lighter gray when clicked
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0F); // No border
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0F); // Slight rounding for hover effect


        for (const auto &suggestion: commandSuggestions_) {
            // Create button with highlighted keyword
            // 创建带有高亮关键字的按钮
            ImGui::PushID(suggestion.c_str());

            // Find keyword position in suggestion (case-insensitive)
            // 在建议中查找关键字位置（不区分大小写）
            size_t keywordPos = std::string::npos;
            if (!keyword_.empty()) {
                std::string lowerSuggestion = suggestion;
                std::string lowerKeyword = keyword_;
                std::transform(lowerSuggestion.begin(), lowerSuggestion.end(), lowerSuggestion.begin(), ::tolower);
                std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
                keywordPos = lowerSuggestion.find(lowerKeyword);
            }

            // Draw button with custom text rendering for highlighting
            // 绘制带有自定义文本渲染的按钮以实现高亮
            const ImVec2 padding = ImGui::GetStyle().FramePadding;

            // Button fills full width (-1 means fill available width)
            // 按钮填充整个宽度（-1 表示填充可用宽度）
            if (ImGui::Button("", ImVec2(-1, 0))) {
                ClikAutoCompleteItem(suggestion);
            }

            // Render text with highlighted keyword on top of button
            // 在按钮上方渲染带有高亮关键字的文本
            const ImVec2 textPos = ImVec2(ImGui::GetItemRectMin().x + padding.x, ImGui::GetItemRectMin().y + padding.y);
            ImDrawList *drawList = ImGui::GetWindowDrawList();

            if (keywordPos != std::string::npos && !keyword_.empty()) {
                // Draw text in three parts: before keyword, keyword (highlighted), after keyword
                // 分三部分绘制文本：关键字前、关键字（高亮）、关键字后
                const std::string beforeKeyword = suggestion.substr(0, keywordPos);
                const std::string keyword = suggestion.substr(keywordPos, keyword_.length());
                const std::string afterKeyword = suggestion.substr(keywordPos + keyword_.length());

                ImVec2 currentPos = textPos;

                // Before keyword - white
                if (!beforeKeyword.empty()) {
                    drawList->AddText(currentPos, IM_COL32(255, 255, 255, 255), beforeKeyword.c_str());
                    currentPos.x += ImGui::CalcTextSize(beforeKeyword.c_str()).x;
                }

                // Keyword - cyan highlight
                drawList->AddText(currentPos, IM_COL32(0, 255, 255, 255), keyword.c_str());
                currentPos.x += ImGui::CalcTextSize(keyword.c_str()).x;

                // After keyword - white
                if (!afterKeyword.empty()) {
                    drawList->AddText(currentPos, IM_COL32(255, 255, 255, 255), afterKeyword.c_str());
                }
            } else {
                // No keyword to highlight, draw entire text in white
                drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), suggestion.c_str());
            }

            ImGui::PopID();
            // Removed ImGui::SameLine() to display suggestions vertically
            // 移除 ImGui::SameLine() 以垂直显示建议
        }

        ImGui::PopStyleVar(2); // Pop FrameBorderSize and FrameRounding
        ImGui::PopStyleColor(3); // Pop Button, ButtonHovered, ButtonActive
        ImGui::EndChild();
    }

    //Command Suggestion Label
    //命令建议标签
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255)); // Light gray for normal text
    for (int i = 0; i < commandStructure_.size(); i++) {
        if (i == commandStructureHighlightIndex_) {
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255)); // Cyan for highlighted
            ImGui::TextUnformatted(commandStructure_[i].c_str());
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255)); // Back to light gray
        } else {
            ImGui::TextUnformatted(commandStructure_[i].c_str());
        }
        if (i < commandStructure_.size() - 1) {
            ImGui::SameLine();
        }
    }

    ImGui::PopStyleColor();
    ImGui::Separator();

    // Set white text color and cursor for input field
    // 为输入框设置白色文本颜色和光标
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_InputTextCursor, IM_COL32(255, 255, 255, 255)); // White cursor

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
    ImGui::PopStyleColor(2); // Pop input text color and cursor color
    ImGui::PopItemWidth();
    ImGui::End();

    // Pop all style colors and vars
    // 弹出所有样式颜色和变量
    ImGui::PopStyleVar(2); // WindowBorderSize, ChildBorderSize
    ImGui::PopStyleColor(12); // All color styles
}


glimmer::ConsoleOverlay::~ConsoleOverlay() = default;
