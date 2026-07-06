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
#include "ConsoleOverlay.h"

#include <algorithm>
#include "AppContext.h"
#include "core/log/LogCat.h"
#include "core/Config.h"
#include "backends/imgui_impl_sdl3.h"
#include "fmt/color.h"
#include "misc/cpp/imgui_stdlib.h"

void glimmer::ConsoleOverlay::SetLastCursorPos(const int cursorPos)
{
    lastCursorPos_ = cursorPos;
}

void glimmer::ConsoleOverlay::SetCommandStructure(const std::vector<std::string>& commandStructure)
{
    commandStructure_ = commandStructure;
}

bool glimmer::ConsoleOverlay::OnBackPressed()
{
    if (show_)
    {
        show_ = false;
        Hide();
        return true;
    }
    return false;
}

void glimmer::ConsoleOverlay::SetKeyword(std::string_view keyword)
{
    keyword_ = keyword;
}

void glimmer::ConsoleOverlay::SetCommandStructureHighlightIndex(const int commandStructureHighlightIndex)
{
    commandStructureHighlightIndex_ = commandStructureHighlightIndex;
}

void glimmer::ConsoleOverlay::SetCommandSuggestions(const std::vector<std::string>& commandSuggestions)
{
    commandSuggestions_.clear();
    selectedSuggestionIndex_ = 0;
    const std::string& keyword = keyword_;

    struct ScoredCommand
    {
        std::string cmd;
        int score;
    };

    std::vector<ScoredCommand> scored;

    for (const auto& cmd : commandSuggestions)
    {
        const int score = ComputeScore(cmd, keyword);
        scored.push_back({cmd, score});
    }

    std::ranges::sort(scored,
                      [](auto& a, auto& b) { return a.score > b.score; });

    commandSuggestions_.reserve(scored.size());
    for (auto& s : scored)
    {
        commandSuggestions_.push_back(std::move(s.cmd));
    }
}

int glimmer::ConsoleOverlay::ComputeScore(std::string_view cmd, std::string_view keyword)
{
    int score = 0;

    if (cmd == keyword) score += 200;
    if (cmd.starts_with(keyword)) score += 100;

    if (auto pos = cmd.find(keyword); pos != std::string::npos)
    {
        score += 10;
        score += 50 - static_cast<int>(pos);
    }

    score -= static_cast<int>(cmd.length());

    return score;
}

int glimmer::ConsoleOverlay::GetLastCursorPos() const
{
    return lastCursorPos_;
}

std::optional<std::string> glimmer::ConsoleOverlay::GetBestHistoryCommandSuggestion() const
{
    const CommandHistoryMessage& commandHistoryMessage = GetAppContext()->GetCommandHistoryManager()->
                                                                      GetCommandHistoryMessage();
    const int historySize = commandHistoryMessage.history_size();
    if (historySize == 0)
    {
        return std::nullopt;
    }
    const google::protobuf::RepeatedPtrField<std::string>& historyList = commandHistoryMessage.history();
    for (int i = historySize - 1; i >= 0; --i)
    {
        const std::string& history = historyList.Get(i);
        if (history.empty())
        {
            continue;
        }
        if (history.starts_with(command_))
        {
            return history;
        }
    }
    return std::nullopt;
}


bool glimmer::ConsoleOverlay::HandleGraveKey()
{
    show_ = !show_;
    if (show_)
    {
        focusNextFrame_ = true;
        Show();
    }
    else
    {
        Hide();
    }
    return true;
}


void glimmer::ConsoleOverlay::NavigateHistory(int direction)
{
    auto& historyMgr = GetAppContext()->GetCommandHistoryManager()->GetCommandHistoryMessage();
    const int history_size = historyMgr.history_size();
    if (history_size <= 0)
    {
        return;
    }

    if (selectedCommandHistoryIndex_ == 0)
    {
        tempCommand_ = command_;
    }

    selectedCommandHistoryIndex_ += direction;
    if (selectedCommandHistoryIndex_ > history_size)
    {
        selectedCommandHistoryIndex_ = 0;
    }
    if (selectedCommandHistoryIndex_ < 0)
    {
        selectedCommandHistoryIndex_ = history_size;
    }

    if (selectedCommandHistoryIndex_ == 0)
    {
        ApplyAutocomplete(tempCommand_);
    }
    else
    {
        const std::string& new_cmd = historyMgr.history().Get(history_size - selectedCommandHistoryIndex_);
        ApplyAutocomplete(new_cmd);
    }
    selectedSuggestionIndex_ = 0;
}

void glimmer::ConsoleOverlay::ApplyAutocomplete(const std::string& text)
{   pendingAutocomplete_ = text;
    lastCursorPos_ = static_cast<int>(text.size());
    nextCursorPos_ = lastCursorPos_;
    focusNextFrame_ = true;
}

bool glimmer::ConsoleOverlay::HandleUpKey()
{
    if (!commandSuggestions_.empty())
    {
        selectedSuggestionIndex_ = selectedSuggestionIndex_ <= 0
                                       ? static_cast<int>(commandSuggestions_.size() - 1)
                                       : selectedSuggestionIndex_ - 1;
        return true;
    }
    NavigateHistory(1);
    return true;
}

bool glimmer::ConsoleOverlay::HandleDownKey()
{
    if (!commandSuggestions_.empty())
    {
        selectedSuggestionIndex_ = selectedSuggestionIndex_ >= static_cast<int>(commandSuggestions_.size()) - 1
                                       ? 0
                                       : selectedSuggestionIndex_ + 1;
        return true;
    }
    NavigateHistory(-1);
    return true;
}

bool glimmer::ConsoleOverlay::HandleRightKey()
{
    if (command_.empty())
    {
        return false;
    }
    if (const int cursorPos = lastCursorPos_ < 0 ? 0 : lastCursorPos_;
        cursorPos != static_cast<int>(command_.length()))
    {
        return false;
    }
    auto suggestion = GetBestHistoryCommandSuggestion();
    if (!suggestion.has_value())
    {
        return false;
    }
    ApplyAutocomplete(suggestion.value());
    return true;
}

bool glimmer::ConsoleOverlay::HandleTabKey()
{
    if (commandSuggestions_.empty())
    {
        return false;
    }
    std::string newCommand = ClikAutoCompleteItem(commandSuggestions_[selectedSuggestionIndex_]);
    ApplyAutocomplete(newCommand);
    commandSuggestions_.clear();
    selectedSuggestionIndex_ = 0;
    return true;
}

bool glimmer::ConsoleOverlay::HandleKeyDown(const SDL_Event& event)
{
    if (event.key.repeat)
    {
        return false;
    }
    const SDL_Keycode keyCode = event.key.key;
    if (keyCode == SDLK_GRAVE)
    {
        return HandleGraveKey();
    }
    if (!show_)
    {
        return false;
    }
    if (keyCode == SDLK_UP)
    {
        return HandleUpKey();
    }
    if (keyCode == SDLK_DOWN)
    {
        return HandleDownKey();
    }
    if (keyCode == SDLK_RIGHT)
    {
        return HandleRightKey();
    }
    if (keyCode == SDLK_TAB)
    {
        return HandleTabKey();
    }
    return false;
}

bool glimmer::ConsoleOverlay::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_EVENT_KEY_DOWN)
    {
        if (HandleKeyDown(event))
        {
            return true;
        }
    }
    ImGui_ImplSDL3_ProcessEvent(&event);
    return show_;
}

void glimmer::ConsoleOverlay::Update(float delta)
{
    for (auto it = commandIDList_.begin(); it != commandIDList_.end();)
    {
        uint32_t commandId = *it;
        std::unique_ptr<CommandResponse> commandResponse = consoleWorker_->TakeCommandResponse(commandId);

        if (commandResponse == nullptr)
        {
            ++it;
            continue;
        }

        CommandResult commandResult = commandResponse->GetCommandResult();
        std::string pattern;
        std::string message;

        switch (commandResult)
        {
        using enum CommandResult;
        case Success:
            pattern = GetAppContext()->GetLangsResources()->executedSuccess;
            break;
        case Failure:
            pattern = GetAppContext()->GetLangsResources()->executionFailed;
            break;
        case EmptyArgs:
            message = GetAppContext()->GetLangsResources()->commandIsEmpty;
            break;
        case NotFound:
            pattern = GetAppContext()->GetLangsResources()->commandNotFound;
            break;
        }

        if (!pattern.empty())
        {
            message = fmt::format(fmt::runtime(pattern), commandResponse->GetCommand());
        }
        addMessage(message);
        it = commandIDList_.erase(it);
    }
}

void glimmer::ConsoleOverlay::OnConfigChanged(const Config* config)
{
    uiScale_ = config->window.uiScale;
}

void glimmer::ConsoleOverlay::PushConsoleStyle(const PreloadColors* preloadColors)
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                      preloadColors->console.backgroundColor.g,
                                                      preloadColors->console.backgroundColor.b,
                                                      preloadColors->console.backgroundColor.a));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                     preloadColors->console.backgroundColor.g,
                                                     preloadColors->console.backgroundColor.b,
                                                     preloadColors->console.backgroundColor.a));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(preloadColors->console.backgroundColor.r,
                                                     preloadColors->console.backgroundColor.g,
                                                     preloadColors->console.backgroundColor.b,
                                                     preloadColors->console.backgroundColor.a));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(40, 40, 40, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(50, 50, 50, 255));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(20, 20, 20, 255));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(80, 80, 80, 255));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(100, 100, 100, 255));
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, IM_COL32(120, 120, 120, 255));
    ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, IM_COL32(60, 60, 60, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0F * uiScale_);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0F * uiScale_);
}

void glimmer::ConsoleOverlay::PopConsoleStyle()
{
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(12);
}

void glimmer::ConsoleOverlay::RenderMessages(float messagesHeight)
{
    ImGui::BeginChild("Messages", ImVec2(0, messagesHeight), true,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);
    {
        std::lock_guard lock(messagesMutex_);
        for (const auto& msg : messages_)
        {
            ImGui::TextUnformatted(msg.c_str());
        }
    }
    if (scrollToBottom_)
    {
        const float scrollY = ImGui::GetScrollY();
        const float scrollMaxY = ImGui::GetScrollMaxY();
        if (scrollMaxY - scrollY < 5.0F)
        {
            ImGui::SetScrollHereY(1.0F);
        }
        scrollToBottom_ = false;
    }
    ImGui::EndChild();
}

void glimmer::ConsoleOverlay::RenderSuggestionItem(size_t index, const std::string& suggestion,
                                                   const PreloadColors* preloadColors)
{
    ImGui::PushID(suggestion.c_str());
    const bool isSelected = static_cast<int>(index) == selectedSuggestionIndex_;

    if (isSelected)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 60, 60, 200));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(60, 60, 60, 200));
    }

    size_t keywordPos = std::string::npos;
    if (!keyword_.empty())
    {
        std::string lowerSuggestion = suggestion;
        std::string lowerKeyword = keyword_;
        std::ranges::transform(lowerSuggestion, lowerSuggestion.begin(), ::tolower);
        std::ranges::transform(lowerKeyword, lowerKeyword.begin(), ::tolower);
        keywordPos = lowerSuggestion.find(lowerKeyword);
    }

    const ImVec2 padding = ImGui::GetStyle().FramePadding;
    if (ImGui::Button(("##" + suggestion).c_str(), ImVec2(-1, 0)))
    {
        ApplyAutocomplete(ClikAutoCompleteItem(suggestion));
        commandSuggestions_.clear();
        selectedSuggestionIndex_ = 0;
    }

    if (ImGui::IsItemHovered())
    {
        selectedSuggestionIndex_ = static_cast<int>(index);
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    const auto textPos = ImVec2(ImGui::GetItemRectMin().x + padding.x,
                                  ImGui::GetItemRectMin().y + padding.y);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const ImU32 textColor = IM_COL32(preloadColors->console.textColor.r,
                                     preloadColors->console.textColor.g,
                                     preloadColors->console.textColor.b,
                                     preloadColors->console.textColor.a);
    const ImU32 keywordColor = IM_COL32(preloadColors->console.keywordColor.r,
                                        preloadColors->console.keywordColor.g,
                                        preloadColors->console.keywordColor.b,
                                        preloadColors->console.keywordColor.a);

    if (keywordPos == std::string::npos || keyword_.empty())
    {
        drawList->AddText(textPos, textColor, suggestion.c_str());
    }
    else
    {
        const std::string beforeKeyword = suggestion.substr(0, keywordPos);
        const std::string keyword = suggestion.substr(keywordPos, keyword_.length());
        const std::string afterKeyword = suggestion.substr(keywordPos + keyword_.length());

        ImVec2 currentPos = textPos;
        if (!beforeKeyword.empty())
        {
            drawList->AddText(currentPos, textColor, beforeKeyword.c_str());
            currentPos.x += ImGui::CalcTextSize(beforeKeyword.c_str()).x;
        }
        drawList->AddText(currentPos, keywordColor, keyword.c_str());
        currentPos.x += ImGui::CalcTextSize(keyword.c_str()).x;
        if (!afterKeyword.empty())
        {
            drawList->AddText(currentPos, textColor, afterKeyword.c_str());
        }
    }

    if (isSelected)
    {
        ImGui::PopStyleColor(2);
    }
    ImGui::PopID();
}

void glimmer::ConsoleOverlay::RenderAutocompleteSuggestions(float suggestionsHeight)
{
    if (commandSuggestions_.empty())
    {
        return;
    }
    ImGui::BeginChild("AutocompleteSuggestions", ImVec2(0, suggestionsHeight), true,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(40, 40, 40, 150));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(60, 60, 60, 200));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0F * GetAppContext()->GetConfig()->window.uiScale);

    for (size_t i = 0; i < commandSuggestions_.size(); ++i)
    {
        RenderSuggestionItem(i, commandSuggestions_[i], GetAppContext()->GetPreloadColors());
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::EndChild();
}

void glimmer::ConsoleOverlay::RenderCommandStructure(const PreloadColors* preloadColors)
{
    const ImU32 textColor = IM_COL32(preloadColors->console.textColor.r,
                                     preloadColors->console.textColor.g,
                                     preloadColors->console.textColor.b,
                                     preloadColors->console.textColor.a);
    const ImU32 keywordColor = IM_COL32(preloadColors->console.keywordColor.r,
                                        preloadColors->console.keywordColor.g,
                                        preloadColors->console.keywordColor.b,
                                        preloadColors->console.keywordColor.a);

    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    for (int i = 0; i < static_cast<int>(commandStructure_.size()); i++)
    {
        if (i == commandStructureHighlightIndex_)
        {
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, keywordColor);
            ImGui::TextUnformatted(commandStructure_[i].c_str());
            ImGui::PopStyleColor();
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        }
        else
        {
            ImGui::TextUnformatted(commandStructure_[i].c_str());
        }
        if (i < static_cast<int>(commandStructure_.size()) - 1)
        {
            ImGui::SameLine();
        }
    }
    ImGui::PopStyleColor();
}

void glimmer::ConsoleOverlay::RenderCommandInput()
{
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    ImGui::PushStyleColor(ImGuiCol_InputTextCursor, IM_COL32(255, 255, 255, 255));
    ImGui::PushItemWidth(-1);

    if (focusNextFrame_)
    {
        ImGui::SetKeyboardFocusHere();
        focusNextFrame_ = false;
    }

    if (ImGui::InputText("##Input", &command_, ImGuiInputTextFlags_EnterReturnsTrue |
                         ImGuiInputTextFlags_CallbackAlways,
                         &ConsoleOverlay::InputCallback, this))
    {
        if (!command_.empty())
        {
            addMessage("> " + command_);
            auto& commandHistoryMessage = GetAppContext()->GetCommandHistoryManager()->GetCommandHistoryMessage();
            const auto mutableHistory = commandHistoryMessage.mutable_history();
            auto new_command = mutableHistory->Add();
            *new_command = command_;
            const uint16_t maxEntries = GetAppContext()->GetConfig()->console.maxHistoryEntries;
            const int current_size = mutableHistory->size();
            if (current_size > maxEntries)
            {
                const int delete_num = current_size - maxEntries;
                mutableHistory->DeleteSubrange(0, delete_num);
            }
            commandIDList_.push_back(consoleWorker_->CreateRequest(command_, &commandSender_));
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

    ImGui::PopStyleColor(2);
    ImGui::PopItemWidth();
}

void glimmer::ConsoleOverlay::RenderHistoryHint(const PreloadColors* preloadColors)
{
    if (command_.empty())
    {
        return;
    }
    const std::optional<std::string> commandSuggestion = GetBestHistoryCommandSuggestion();
    if (!commandSuggestion.has_value())
    {
        return;
    }
    ImDrawList* drawList = ImGui::GetWindowDrawList();
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

void glimmer::ConsoleOverlay::RenderImGui(SDL_Renderer* renderer)
{
    if (!show_)
    {
        return;
    }
    const PreloadColors* preloadColors = GetAppContext()->GetPreloadColors();
    ImGui::GetIO().FontGlobalScale = uiScale_;
    const ImGuiIO& io = ImGui::GetIO();
    const float windowHeight = io.DisplaySize.y;
    const float inputHeight = 25.0F * uiScale_;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);

    PushConsoleStyle(preloadColors);

    ImGui::Begin("Console",
                 nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushFont(ImGui::GetFont());
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(preloadColors->console.keywordColor.r,
                                                  preloadColors->console.keywordColor.g,
                                                  preloadColors->console.keywordColor.b,
                                                  preloadColors->console.keywordColor.a));
    ImGui::TextUnformatted(GetAppContext()->GetLangsResources()->console.c_str());
    ImGui::PopStyleColor();
    ImGui::PopFont();
    ImGui::Separator();

    float suggestionsHeight = 0.0F;
    if (!commandSuggestions_.empty())
    {
        const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        const float desiredHeight = lineHeight * static_cast<float>(commandSuggestions_.size());
        const float maxSuggestionsHeight = windowHeight * 0.4F;
        const float extraPadding = ImGui::GetStyle().FramePadding.y * 2 +
            ImGui::GetStyle().ItemSpacing.y * 2 +
            ImGui::GetStyle().ScrollbarSize;
        suggestionsHeight = std::min(desiredHeight + extraPadding, maxSuggestionsHeight);
    }

    const float messagesHeight = windowHeight - inputHeight - 70 - suggestionsHeight;

    RenderMessages(messagesHeight);
    RenderAutocompleteSuggestions(suggestionsHeight);
    RenderCommandStructure(preloadColors);
    ImGui::Separator();
    RenderCommandInput();
    RenderHistoryHint(preloadColors);

    ImGui::End();
    PopConsoleStyle();
}


void glimmer::ConsoleOverlay::addMessage(const std::string& message)
{
    LogCat::d(message);
    std::lock_guard lock(messagesMutex_);
    messages_.push_back(message);
    scrollToBottom_ = true;
}

int glimmer::ConsoleOverlay::InputCallback(ImGuiInputTextCallbackData* data)
{
    //When the text changes
    //当文本改变时
    auto overlay = static_cast<ConsoleOverlay*>(data->UserData);
    const int cursorPos = data->CursorPos;
    const std::string cmdStr(data->Buf, data->BufTextLen);
    if (cursorPos != overlay->GetLastCursorPos())
    {
        overlay->SetLastCursorPos(cursorPos);
        const auto commandArgs = CommandArgs(cmdStr);
        const auto keyword = commandArgs.GetKeywordAtCursor(cursorPos);
        overlay->SetKeyword(keyword);
        overlay->SetCommandSuggestions(
            overlay->GetAppContext()->GetCommandManager()->GetSuggestions(
                overlay->GetAppContext()->GetDynamicSuggestionsManager(),
                commandArgs, cursorPos));
        overlay->SetCommandStructure(overlay->GetAppContext()->GetCommandManager()->GetCommandStructure(&commandArgs));
        overlay->SetCommandStructureHighlightIndex(commandArgs.GetTokenIndexAtCursor(cursorPos));
    }
    if (overlay->nextCursorPos_ != -1)
    {
        data->CursorPos = overlay->nextCursorPos_;
        data->SelectionStart = overlay->nextCursorPos_;
        data->SelectionEnd = overlay->nextCursorPos_;
        overlay->nextCursorPos_ = -1;
        //Set it to -1 to update the command structure and the list of command suggestions
        //设置为-1，以便更新命令结构和命令建议列表
        overlay->SetLastCursorPos(-1);
    }
    if (overlay->pendingAutocomplete_.has_value())
    {
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, overlay->pendingAutocomplete_->c_str());
        data->CursorPos = data->BufTextLen;
        overlay->pendingAutocomplete_.reset();
        overlay->commandSuggestions_.clear();
    }
    return 0;
}

std::string glimmer::ConsoleOverlay::ClikAutoCompleteItem(std::string_view suggestion) const
{
    const int cursorPos = lastCursorPos_;
    int leftSpacePos = -1;
    for (int i = cursorPos - 1; i >= 0; --i)
    {
        if (command_[i] == ' ')
        {
            leftSpacePos = i;
            break;
        }
    }

    std::string beforeCursor;
    if (leftSpacePos >= 0)
    {
        beforeCursor = command_.substr(0, leftSpacePos + 1);
    }

    int rightSpacePos = -1;
    for (int i = cursorPos; i < static_cast<int>(command_.size()); ++i)
    {
        if (command_[i] == ' ')
        {
            rightSpacePos = i;
            break;
        }
    }

    std::string afterCursor;
    if (rightSpacePos >= 0)
    {
        afterCursor = command_.substr(rightSpacePos);
    }

    std::string newText;
    newText.reserve(beforeCursor.size() + suggestion.size() + afterCursor.size());
    newText.append(beforeCursor);
    newText.append(suggestion);
    newText.append(afterCursor);
    return newText;
}

void glimmer::ConsoleOverlay::Show()
{
    if (consoleWorker_ == nullptr)
    {
        return;
    }
    consoleWorker_->PushOnMessage(
        std::make_unique<std::function<void(const std::string&)>>([this](const std::string& text)
        {
            this->addMessage(text);
        })
    );
}

void glimmer::ConsoleOverlay::Hide() const
{
    if (consoleWorker_ != nullptr)
    {
        consoleWorker_->PopOnMessage();
    }
}


glimmer::ConsoleOverlay::ConsoleOverlay(AppContext* context)
    : Scene(context),
      consoleWorker_(context->GetConsoleWorker())
{
    Init();
}


glimmer::ConsoleOverlay::~ConsoleOverlay() = default;
