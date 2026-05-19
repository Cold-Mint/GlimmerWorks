//
// Created by Cold-Mint on 2025/10/13.
//

#pragma once
#include <string>
#include <mutex>
#include <optional>
#include <vector>

#include "imgui.h"
#include "Scene.h"
#include "core/console/ConsoleWorker.h"

namespace glimmer {
    /**
     * ConsoleOverlay
     * 控制台叠加层
     *
     */
    class ConsoleOverlay : public Scene {
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
        ConsoleWorker *consoleWorker_;
        CommandSender commandSender_;

        void addMessage(const std::string &message);

        static int InputCallback(ImGuiInputTextCallbackData *data);

        [[nodiscard]] std::string ClikAutoCompleteItem(const std::string &suggestion) const;

        void Show();

        void Hide() const;

    public:
        explicit ConsoleOverlay(AppContext *context);

        void SetLastCursorPos(int cursorPos);

        void SetCommandStructure(const std::vector<std::string> &commandStructure);

        bool OnBackPressed() override;

        /**
         * Set the keywords when searching
         * 设置搜索时的关键字
         * @param keyword keyword 关键字
         */
        void SetKeyword(const std::string &keyword);

        void SetCommandStructureHighlightIndex(int commandStructureHighlightIndex);

        void SetCommandSuggestions(const std::vector<std::string> &commandSuggestions);

        static int ComputeScore(const std::string &cmd, const std::string &keyword);

        [[nodiscard]] int GetLastCursorPos() const;

        [[nodiscard]] std::optional<std::string> GetBestHistoryCommandSuggestion() const;

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;


        void Render(SDL_Renderer *renderer) override;

        ~ConsoleOverlay() override;
    };
}
