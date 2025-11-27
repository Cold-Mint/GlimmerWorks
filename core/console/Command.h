//
// Created by Cold-Mint on 2025/10/15.
//

#ifndef GLIMMERWORKS_COMMAND_H
#define GLIMMERWORKS_COMMAND_H
#include <functional>
#include <string>

#include "CommandArgs.h"
#include "../utils/NodeTree.h"

namespace glimmer {
    class WorldContext;
    class AppContext;

    class Command {
    protected:
        AppContext *appContext_ = nullptr;
        WorldContext *worldContext_ = nullptr;
        /**
         * A tree applicable to command suggestions
         * 适用于命令建议的树
         */
        NodeTree<std::string> suggestionsTree_ = NodeTree<std::string>();

        /**
        * Initialize the suggestion tree
        * 初始化建议树
        * @param suggestionsTree
        */
        virtual void InitSuggestions(NodeTree<std::string> &suggestionsTree) = 0;

    public:
        virtual ~Command() = default;


        explicit Command(AppContext *ctx) : appContext_(ctx) {
        }

        [[nodiscard]] virtual std::string GetName() const = 0;

        /**
         * Is the world context required to execute this command
         * 是否需要世界上下文，才能执行该命令
         * @return 是否需要世界上下文
         */
        [[nodiscard]] virtual bool RequiresWorldContext() const;

        /**
         * Bind the world context
         * 绑定世界上下文
         * @param worldContext 世界上下文
         */
        void BindWorldContext(WorldContext *worldContext);

        /**
         * UnBind the world context
         * 解绑世界上下文
         */
        void UnBindWorldContext();


        /**
         * Add command structure
         * 添加命令结构
         * @param commandArgs commandArgs 命令参数
         * @param strings Command structure array
         */
        virtual void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) = 0;

        /**
         * Initialize
         * 初始化
         */
        void Initialize() {
            InitSuggestions(suggestionsTree_);
        }

        [[nodiscard]] NodeTree<std::string> GetSuggestionsTree() const;

        virtual bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onOutput) = 0;
    };
}


#endif //GLIMMERWORKS_COMMAND_H
