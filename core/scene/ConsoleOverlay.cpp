//
// Created by Cold-Mint on 2025/10/13.
//

#include "ConsoleOverlay.h"

#include <algorithm>
#include "AppContext.h"
#include "../log/LogCat.h"
#include "../Config.h"
#include "backends/imgui_impl_sdl3.h"
#include "fmt/color.h"
#include "misc/cpp/imgui_stdlib.h"

void glimmer::ConsoleOverlay::SetLastCursorPos(const int cursorPos) {
    lastCursorPos_ = cursorPos;
}

void glimmer::ConsoleOverlay::SetCommandStructure(const std::vector<std::string> &commandStructure) {
    commandStructure_ = commandStructure;
}

bool glimmer::ConsoleOverlay::OnBackPressed() {
    if (show_) {
        show_ = false;
        return true;
    }
    return false;
}

void glimmer::ConsoleOverlay::SetKeyword(const std::string &keyword) {
    keyword_ = keyword;
}

void glimmer::ConsoleOverlay::SetCommandStructureHighlightIndex(const int commandStructureHighlightIndex) {
    commandStructureHighlightIndex_ = commandStructureHighlightIndex;
}

void glimmer::ConsoleOverlay::SetCommandSuggestions(const std::vector<std::string> &commandSuggestions) {
    commandSuggestions_.clear();
    selectedSuggestionIndex_ = 0;
    const std::string &keyword = keyword_;

    struct ScoredCommand {
        std::string cmd;
        int score;
    };

    std::vector<ScoredCommand> scored;

    for (const auto &cmd: commandSuggestions) {
        const int score = ComputeScore(cmd, keyword);
        scored.push_back({cmd, score});
    }

    std::ranges::sort(scored,
                      [](auto &a, auto &b) { return a.score > b.score; });

    commandSuggestions_.reserve(scored.size());
    for (auto &s: scored) {
        commandSuggestions_.push_back(std::move(s.cmd));
    }
}

int glimmer::ConsoleOverlay::ComputeScore(const std::string &cmd, const std::string &keyword) {
    int score = 0;

    if (cmd == keyword) score += 200;
    if (cmd.starts_with(keyword)) score += 100;

    if (auto pos = cmd.find(keyword); pos != std::string::npos) {
        score += 10;
        score += 50 - static_cast<int>(pos);
    }

    score -= static_cast<int>(cmd.length());

    return score;
}

int glimmer::ConsoleOverlay::GetLastCursorPos() const {
    return lastCursorPos_;
}

std::optional<std::string> glimmer::ConsoleOverlay::GetBestHistoryCommandSuggestion() const {
    const CommandHistoryMessage &commandHistoryMessage = appContext->GetCommandHistoryManager()->
            GetCommandHistoryMessage();
    const int historySize = commandHistoryMessage.history_size();
    if (historySize == 0) {
        return std::nullopt;
    }
    const google::protobuf::RepeatedPtrField<std::string> &historyList = commandHistoryMessage.history();
    for (int i = historySize - 1; i >= 0; --i) {
        const std::string &history = historyList.Get(i);
        if (history.empty()) {
            continue;
        }
        if (history.starts_with(command_)) {
            return history;
        }
    }
    return std::nullopt;
}


bool glimmer::ConsoleOverlay::HandleEvent(const SDL_Event &event) {
    if (!event.key.repeat && event.type == SDL_EVENT_KEY_DOWN) {
        const SDL_Keycode keyCode = event.key.key;
        if (keyCode == SDLK_GRAVE) {
            show_ = !show_;
            if (show_) {
                focusNextFrame_ = true;
            }
            return true;
        }
        if (show_ && keyCode == SDLK_UP) {
            if (!commandSuggestions_.empty()) {
                selectedSuggestionIndex_ = selectedSuggestionIndex_ <= 0
                                               ? static_cast<int>(commandSuggestions_.size() - 1)
                                               : selectedSuggestionIndex_ - 1;
                return true;
            }
            auto &historyMgr = appContext->GetCommandHistoryManager()->GetCommandHistoryMessage();
            const int history_size = historyMgr.history_size();
            if (history_size <= 0) return true;
            if (selectedCommandHistoryIndex_ == 0) {
                tempCommand_ = command_;
            }
            if (selectedCommandHistoryIndex_ < history_size) {
                selectedCommandHistoryIndex_++;
                const std::string &new_cmd = historyMgr.history().Get(
                    history_size - selectedCommandHistoryIndex_
                );
                pendingAutocomplete_ = new_cmd;
                lastCursorPos_ = static_cast<int>(new_cmd.size());
                nextCursorPos_ = lastCursorPos_;
                selectedSuggestionIndex_ = 0;
                focusNextFrame_ = true;
            }
            return true;
        }

        if (show_ && keyCode == SDLK_DOWN) {
            if (!commandSuggestions_.empty()) {
                selectedSuggestionIndex_ = selectedSuggestionIndex_ >= static_cast<int>(commandSuggestions_.size()) - 1
                                               ? 0
                                               : selectedSuggestionIndex_ + 1;
                return true;
            }

            auto &historyMgr = appContext->GetCommandHistoryManager()->GetCommandHistoryMessage();
            const int history_size = historyMgr.history_size();
            if (history_size <= 0 || selectedCommandHistoryIndex_ <= 0) return true;

            selectedCommandHistoryIndex_--;
            if (selectedCommandHistoryIndex_ == 0) {
                pendingAutocomplete_ = tempCommand_;
                lastCursorPos_ = static_cast<int>(tempCommand_.size());
            } else {
                const std::string &new_cmd = historyMgr.history().Get(
                    history_size - selectedCommandHistoryIndex_
                );
                pendingAutocomplete_ = new_cmd;
                lastCursorPos_ = static_cast<int>(new_cmd.size());
            }

            nextCursorPos_ = lastCursorPos_;
            selectedSuggestionIndex_ = 0;
            focusNextFrame_ = true;
            return true;
        }
        if (show_ && keyCode == SDLK_RIGHT && !command_.empty()) {
            const int cursorPos = lastCursorPos_ < 0 ? 0 : lastCursorPos_;
            if (cursorPos == static_cast<int>(command_.length())) {
                auto suggestion = GetBestHistoryCommandSuggestion();
                if (suggestion.has_value()) {
                    pendingAutocomplete_ = suggestion.value();
                    lastCursorPos_ = static_cast<int>(suggestion->size());
                    nextCursorPos_ = lastCursorPos_;
                    focusNextFrame_ = true;
                    return true;
                }
            }
            return false;
        }
        if (show_ && keyCode == SDLK_TAB && !commandSuggestions_.empty()) {
            std::string newCommand = ClikAutoCompleteItem(commandSuggestions_[selectedSuggestionIndex_]);
            pendingAutocomplete_ = newCommand;
            lastCursorPos_ = static_cast<int>(newCommand.size());
            nextCursorPos_ = lastCursorPos_;
            commandSuggestions_.clear();
            selectedSuggestionIndex_ = 0;
            focusNextFrame_ = true;
            return true;
        }
    }
    ImGui_ImplSDL3_ProcessEvent(&event);
    return show_;
}

void glimmer::ConsoleOverlay::Update(float delta) {
}

void glimmer::ConsoleOverlay::addMessage(const std::string &message) {
    std::lock_guard lock(messagesMutex_);
    messages_.push_back(message);
    scrollToBottom_ = true;
}

int glimmer::ConsoleOverlay::InputCallback(ImGuiInputTextCallbackData *data) {
    //When the text changes
    //当文本改变时
    auto *overlay = static_cast<ConsoleOverlay *>(data->UserData);
    const int cursorPos = data->CursorPos;
    const std::string cmdStr(data->Buf, data->BufTextLen);
    if (cursorPos != overlay->GetLastCursorPos()) {
        overlay->SetLastCursorPos(cursorPos);
        const auto commandArgs = CommandArgs(cmdStr);
        const auto keyword = commandArgs.GetKeywordAtCursor(cursorPos);
        overlay->SetKeyword(keyword);
        overlay->SetCommandSuggestions(
            overlay->appContext->GetCommandManager()->GetSuggestions(
                overlay->appContext->GetDynamicSuggestionsManager(),
                commandArgs, cursorPos));
        overlay->SetCommandStructure(overlay->appContext->GetCommandManager()->GetCommandStructure(commandArgs));
        overlay->SetCommandStructureHighlightIndex(commandArgs.GetTokenIndexAtCursor(cursorPos));
    }
    if (overlay->nextCursorPos_ != -1) {
        data->CursorPos = overlay->nextCursorPos_;
        data->SelectionStart = overlay->nextCursorPos_;
        data->SelectionEnd = overlay->nextCursorPos_;
        overlay->nextCursorPos_ = -1;
        //Set it to -1 to update the command structure and the list of command suggestions
        //设置为-1，以便更新命令结构和命令建议列表
        overlay->SetLastCursorPos(-1);
    }
    if (overlay->pendingAutocomplete_.has_value()) {
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, overlay->pendingAutocomplete_->c_str());
        data->CursorPos = data->BufTextLen;
        overlay->pendingAutocomplete_.reset();
        overlay->commandSuggestions_.clear();
    }
    return 0;
}

std::string glimmer::ConsoleOverlay::ClikAutoCompleteItem(const std::string &suggestion) const {
    const int cursorPos = lastCursorPos_;
    int leftSpacePos = -1;
    for (int i = cursorPos - 1; i >= 0; --i) {
        if (command_[i] == ' ') {
            leftSpacePos = i;
            break;
        }
    }

    std::string beforeCursor;
    if (leftSpacePos >= 0) {
        beforeCursor = command_.substr(0, leftSpacePos + 1);
    }

    int rightSpacePos = -1;
    for (int i = cursorPos; i < static_cast<int>(command_.size()); ++i) {
        if (command_[i] == ' ') {
            rightSpacePos = i;
            break;
        }
    }

    std::string afterCursor;
    if (rightSpacePos >= 0) {
        afterCursor = command_.substr(rightSpacePos);
    }

    std::string newText;
    newText.reserve(beforeCursor.size() + suggestion.size() + afterCursor.size());
    newText.append(beforeCursor);
    newText.append(suggestion);
    newText.append(afterCursor);
    return newText;
}


glimmer::ConsoleOverlay::ConsoleOverlay(AppContext *context)
    : Scene(context) {
}

void glimmer::ConsoleOverlay::Render(SDL_Renderer *renderer) {
    if (!show_) return;
    const float uiScale = appContext->GetConfig()->window.uiScale;
    const PreloadColors *preloadColors = appContext->GetPreloadColors();
    ImGui::GetIO().FontGlobalScale = uiScale;
    const ImGuiIO &io = ImGui::GetIO();
    const float windowHeight = io.DisplaySize.y;
    const float inputHeight = 25.0F * uiScale;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    // Apply dark console theme
    // 应用深色控制台主题
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                      preloadColors->console.backgroundColor.g,
                                                      preloadColors->console.backgroundColor.b,
                                                      preloadColors->console.backgroundColor.a));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(60, 60, 60, 255)); // Dark gray border
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255)); // White text
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                     preloadColors->console.backgroundColor.g,
                                                     preloadColors->console.backgroundColor.b,
                                                     preloadColors->console.backgroundColor.a));
    // Slightly lighter black for child windows
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                     preloadColors->console.backgroundColor.g,
                                                     preloadColors->console.backgroundColor.b,
                                                     preloadColors->console.backgroundColor.a));
    // Dark gray for input background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(40, 40, 40, 255)); // Lighter on hover
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(50, 50, 50, 255)); // Even lighter when active
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(20, 20, 20, 255)); // Dark scrollbar background
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(80, 80, 80, 255)); // Gray scrollbar grab
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(100, 100, 100, 255)); // Lighter on hover
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(120, 120, 120, 255)); // Even lighter when active
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(60, 60, 60, 255)); // Dark gray for text selection
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0F * uiScale); // Window border
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0F * uiScale); // Child window border

    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Console title
    // 控制台标题
    ImGui::PushFont(ImGui::GetFont());
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(preloadColors->console.keywordColor.r,
                                                  preloadColors->console.keywordColor.g,
                                                  preloadColors->console.keywordColor.b,
                                                  preloadColors->console.keywordColor.a)); // Cyan for title
    ImGui::TextUnformatted(appContext->GetLangsResources()->console.c_str());
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
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);
    //使用标准循环代替ImGuiListClipper，以避免可变高度项目的滚动跳跃
    //同时使用互斥锁保护线程安全
    // Use a standard loop instead of ImGuiListClipper to avoid scroll jumping with variable height items
    // Also protect with mutex for thread safety
    {
        std::lock_guard lock(messagesMutex_);
        for (const auto &msg: messages_) {
            ImGui::TextUnformatted(msg.c_str());
        }
    }

    if (scrollToBottom_) {
        const float scrollY = ImGui::GetScrollY();
        const float scrollMaxY = ImGui::GetScrollMaxY();
        // If the distance from the bottom is less than a certain threshold (for example, 5 pixels), it is considered that "the user is at the bottom".
        // 如果距离底部小于一定阈值（比如 5 像素），则认为“用户在底部”
        if (scrollMaxY - scrollY < 5.0F) {
            ImGui::SetScrollHereY(1.0F);
        }
        scrollToBottom_ = false;
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
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0F * appContext->GetConfig()->window.uiScale);
        for (size_t i = 0; i < commandSuggestions_.size(); ++i) {
            const auto &suggestion = commandSuggestions_[i];
            ImGui::PushID(suggestion.c_str());

            bool isSelected = static_cast<int>(i) == selectedSuggestionIndex_;
            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 60, 60, 200));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 60, 200)); // hover时保持选中样式
            }
            size_t keywordPos = std::string::npos;
            if (!keyword_.empty()) {
                std::string lowerSuggestion = suggestion;
                std::string lowerKeyword = keyword_;
                std::ranges::transform(lowerSuggestion, lowerSuggestion.begin(), ::tolower);
                std::ranges::transform(lowerKeyword, lowerKeyword.begin(), ::tolower);
                keywordPos = lowerSuggestion.find(lowerKeyword);
            }
            const ImVec2 padding = ImGui::GetStyle().FramePadding;
            if (ImGui::Button(("##" + suggestion).c_str(), ImVec2(-1, 0))) {
                std::string newCommand = ClikAutoCompleteItem(suggestion);
                pendingAutocomplete_ = newCommand;
                lastCursorPos_ = static_cast<int>(newCommand.size());
                nextCursorPos_ = lastCursorPos_;
                commandSuggestions_.clear();
                selectedSuggestionIndex_ = 0;
                focusNextFrame_ = true;
            }
            if (ImGui::IsItemHovered()) {
                // When the mouse hovers over the current suggestion item, the selected index will be updated directly.
                // 鼠标悬停在当前建议项上，直接更新选中索引
                selectedSuggestionIndex_ = static_cast<int>(i);
            }
            const ImVec2 textPos = ImVec2(ImGui::GetItemRectMin().x + padding.x, ImGui::GetItemRectMin().y + padding.y);
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            if (keywordPos != std::string::npos && !keyword_.empty()) {
                const std::string beforeKeyword = suggestion.substr(0, keywordPos);
                const std::string keyword = suggestion.substr(keywordPos, keyword_.length());
                const std::string afterKeyword = suggestion.substr(keywordPos + keyword_.length());

                ImVec2 currentPos = textPos;
                if (!beforeKeyword.empty()) {
                    drawList->AddText(currentPos, IM_COL32(preloadColors->console.textColor.r,
                                                           preloadColors->console.textColor.g,
                                                           preloadColors->console.textColor.b,
                                                           preloadColors->console.textColor.a), beforeKeyword.c_str());
                    currentPos.x += ImGui::CalcTextSize(beforeKeyword.c_str()).x;
                }
                drawList->AddText(currentPos, IM_COL32(preloadColors->console.keywordColor.r,
                                                       preloadColors->console.keywordColor.g,
                                                       preloadColors->console.keywordColor.b,
                                                       preloadColors->console.keywordColor.a), keyword.c_str());
                currentPos.x += ImGui::CalcTextSize(keyword.c_str()).x;
                if (!afterKeyword.empty()) {
                    drawList->AddText(currentPos, IM_COL32(preloadColors->console.textColor.r,
                                                           preloadColors->console.textColor.g,
                                                           preloadColors->console.textColor.b,
                                                           preloadColors->console.textColor.a), afterKeyword.c_str());
                }
            } else {
                drawList->AddText(textPos, IM_COL32(preloadColors->console.textColor.r,
                                                    preloadColors->console.textColor.g,
                                                    preloadColors->console.textColor.b,
                                                    preloadColors->console.textColor.a), suggestion.c_str());
            }
            if (isSelected) {
                ImGui::PopStyleColor(2);
            }

            ImGui::PopID();
        }

        ImGui::PopStyleVar(2); // Pop FrameBorderSize and FrameRounding
        ImGui::PopStyleColor(3); // Pop Button, ButtonHovered, ButtonActive
        ImGui::EndChild();
    }

    //Command Suggestion Label
    //命令建议标签
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(preloadColors->console.textColor.r,
                                                  preloadColors->console.textColor.g,
                                                  preloadColors->console.textColor.b,
                                                  preloadColors->console.textColor.a)); // Light gray for normal text
    for (int i = 0; i < commandStructure_.size(); i++) {
        if (i == commandStructureHighlightIndex_) {
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(preloadColors->console.keywordColor.r,
                                                          preloadColors->console.keywordColor.g,
                                                          preloadColors->console.keywordColor.b,
                                                          preloadColors->console.keywordColor.a));
            ImGui::TextUnformatted(commandStructure_[i].c_str());
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(preloadColors->console.textColor.r,
                                                          preloadColors->console.textColor.g,
                                                          preloadColors->console.textColor.b,
                                                          preloadColors->console.textColor.a));
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
    if (ImGui::InputText("##Input", &command_, ImGuiInputTextFlags_EnterReturnsTrue |
                                               ImGuiInputTextFlags_CallbackAlways,
                         &ConsoleOverlay::InputCallback, this)) {
        //It is executed when the player presses the Enter key.
        //当玩家按下Enter键后执行。
        if (!command_.empty()) {
            addMessage("> " + command_);
            auto &commandHistoryMessage = appContext->GetCommandHistoryManager()->GetCommandHistoryMessage();
            const auto mutableHistory = commandHistoryMessage.mutable_history();
            auto *new_command = mutableHistory->Add();
            *new_command = command_;
            const int max_entries = appContext->GetConfig()->console.maxHistoryEntries;
            int current_size = mutableHistory->size();
            if (current_size > max_entries && max_entries >= 0) {
                const int delete_num = current_size - max_entries;
                mutableHistory->DeleteSubrange(0, delete_num);
            }
            CommandExecutor::ExecuteAsyncSingle(command_, appContext->GetCommandManager(),
                                                [this](const CommandResult result, const std::string &cmd) {
                                                    std::string message;
                                                    std::string pattern;
                                                    switch (result) {
                                                        case CommandResult::Success:
                                                            pattern = appContext->GetLangsResources()->executedSuccess;
                                                            break;
                                                        case CommandResult::Failure:
                                                            pattern = appContext->GetLangsResources()->executionFailed;
                                                            break;
                                                        case CommandResult::EmptyArgs:
                                                            message = appContext->GetLangsResources()->commandIsEmpty;
                                                            break;
                                                        case CommandResult::NotFound:
                                                            pattern = appContext->GetLangsResources()->commandNotFound;
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
        }
        command_.clear();
        tempCommand_.clear();
        selectedCommandHistoryIndex_ = 0;
        selectedSuggestionIndex_ = 0;
#ifdef __ANDROID__
        focusNextFrame_ = false;
#else
        focusNextFrame_ = true;
#endif
    }


    //Draw the best recommendations based on the command history.
    //绘制最佳建议，基于命令历史。
    if (!command_.empty()) {
        const std::optional<std::string> commandSuggestion = GetBestHistoryCommandSuggestion();
        if (commandSuggestion.has_value()) {
            ImDrawList *drawList = ImGui::GetWindowDrawList();
            const ImVec2 input_rect_min = ImGui::GetItemRectMin();
            const ImVec2 padding = ImGui::GetStyle().FramePadding;
            const ImVec2 text_pos = ImVec2(
                input_rect_min.x + padding.x,
                input_rect_min.y + padding.y
            );
            const ImU32 hint_color = IM_COL32(
                preloadColors->console.textColor.r,
                preloadColors->console.textColor.g,
                preloadColors->console.textColor.b,
                160
            );
            drawList->AddText(text_pos, hint_color, commandSuggestion.value().c_str());
        }
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
