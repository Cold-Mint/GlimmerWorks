//
// Created by Cold-Mint on 2025/10/13.
//

#ifndef GLIMMERWORKS_CONSOLESCENE_H
#define GLIMMERWORKS_CONSOLESCENE_H
#include <array>
#include <string>
#include <mutex>
#include <vector>

#include "imgui.h"
#include "Scene.h"

namespace glimmer {
    /**
     * ConsoleOverlay
     * 控制台叠加层
     *
     */
    class ConsoleOverlay : public Scene {
        bool show_ = false;
        bool focusNextFrame_ = false;
        std::array<char, 256> inputBuffer_ = {};
        std::mutex messagesMutex_;
        std::vector<std::string> messages_;
        std::vector<std::string> commandSuggestions_;
        std::string keyword_;
        int lastCursorPos_ = 0;
        int nextCursorPos_ = -1;
        bool scrollToBottom_ = false;
        /**
         * Code structure
         * 命令结构
         */
        std::vector<std::string> commandStructure_ = {"[command name]"};
        /**
         * The command structure requires a highlighted index position
         * 命令结构需要高亮的索引位置
         */
        int commandStructureHighlightIndex_ = 0;

        void addMessage(const std::string &message);

        static int InputCallback(ImGuiInputTextCallbackData *data);

        void ClikAutoCompleteItem(const std::string &suggestion);

    public:
        explicit ConsoleOverlay(AppContext *context)
            : Scene(context) {
        }

        void SetLastCursorPos(int cursorPos);

        void SetCommandStructure(const std::vector<std::string> &commandStructure);

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

        bool HandleEvent(const SDL_Event &event) override;

        void Update(float delta) override;


        void Render(SDL_Renderer *renderer) override;

        ~ConsoleOverlay() override;
    };
}

#endif //GLIMMERWORKS_CONSOLESCENE_H
