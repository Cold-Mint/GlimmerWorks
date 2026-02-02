//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMANDMANAGER_H
#define GLIMMERWORKS_COMMANDMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "Command.h"
#include "suggestion/DynamicSuggestionsManager.h"

namespace glimmer {
    class CommandManager {
        std::unordered_map<std::string, std::unique_ptr<Command> > commandMap_{};

    public:
        void RegisterCommand(std::unique_ptr<Command> command);

        Command *GetCommand(const std::string &name) const;

        std::vector<std::string> GetSuggestions(const DynamicSuggestionsManager *dynamicSuggestionsManager,
                                                const CommandArgs &commandArgs, int cursorPos) const;

        /**
         * Get Command Structure
         * 获取命令结构
         * @param commandArgs commandArgs 命令参数
         * @return
         */
        std::vector<std::string> GetCommandStructure(const CommandArgs &commandArgs) const;

        /**
         * Extended suggestions
         * 扩展建议
         * @param dynamicSuggestionsManager Dynamic Suggestion Manager 动态建议管理器
         * @param nextNodeTree Fully expand the dynamic suggestions of the nodes. 将节点的动态建议完全展开。
         * @return The expanded list of suggestions 展开后的建议列表
         */
        static std::vector<std::string> ExtendSuggestions(const DynamicSuggestionsManager *dynamicSuggestionsManager,
                                                          NodeTree<std::string> *nextNodeTree);

        using CommandMap = std::unordered_map<std::string, std::unique_ptr<Command> >;

        [[nodiscard]] const CommandMap &GetCommands();
    };
}

#endif //GLIMMERWORKS_COMMANDMANAGER_H
