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
#pragma once
#include <string>
#include <mutex>
#include <optional>
#include <vector>

#include "imgui.h"
#include "Scene.h"
#include "core/console/ConsoleWorker.h"

namespace glimmer
{
    /**
     * ConsoleOverlay
     * 控制台叠加层
     *
     */
    class ConsoleOverlay : public Scene
    {
        bool show_ = false;
        bool focusNextFrame_ = false;
        std::string command_;
        std::optional<std::string> pendingAutocomplete_;
        std::mutex messagesMutex_;
        std::vector<std::string> messages_;
        std::vector<std::string> commandSuggestions_;
        std::string keyword_;
        int lastCursorPos_ = 0;
        int nextCursorPos_ = -1;
        bool scrollToBottom_ = false;
        // Selected command suggestion item index
        // 选中的命令建议项索引
        int selectedSuggestionIndex_ = 0;
        int selectedCommandHistoryIndex_ = 0;
        // Temporarily save the unsubmitted input content (used when switching between the history)
        // 临时保存未提交的输入内容（上下切换历史时用）
        std::string tempCommand_;
        std::vector<uint32_t> commandIDList_;
        /**
         * Code structure
         * 命令结构
         */
        std::vector<std::string> commandStructure_ = {"[command name:string]"};
        /**
         * The command structure requires a highlighted index position
         * 命令结构需要高亮的索引位置
         */
        int commandStructureHighlightIndex_ = 0;
        ConsoleWorker* consoleWorker_;
        CommandSender commandSender_;
        float uiScale_ = 1.0F;

        void addMessage(const std::string& message);

        static int InputCallback(ImGuiInputTextCallbackData* data);

        [[nodiscard]] std::string ClikAutoCompleteItem(const std::string& suggestion) const;

        void Show();

        void Hide() const;

    public:
        explicit ConsoleOverlay(AppContext* context);

        void SetLastCursorPos(int cursorPos);

        void SetCommandStructure(const std::vector<std::string>& commandStructure);

        bool OnBackPressed() override;

        /**
         * Set the keywords when searching
         * 设置搜索时的关键字
         * @param keyword keyword 关键字
         */
        void SetKeyword(const std::string& keyword);

        void SetCommandStructureHighlightIndex(int commandStructureHighlightIndex);

        void SetCommandSuggestions(const std::vector<std::string>& commandSuggestions);

        static int ComputeScore(const std::string& cmd, const std::string& keyword);

        [[nodiscard]] int GetLastCursorPos() const;

        [[nodiscard]] std::optional<std::string> GetBestHistoryCommandSuggestion() const;

        bool HandleEvent(const SDL_Event& event) override;

        void Update(float delta) override;

        void OnConfigChanged(const Config* config) override;

        void RenderImGui(int width, int height, SDL_Renderer* renderer) override;

        ~ConsoleOverlay() override;
    };
}
