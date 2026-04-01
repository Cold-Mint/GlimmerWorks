//
// Created by coldmint on 2026/3/29.
//

#ifndef GLIMMERWORKS_ECHOCOMMAND_H
#define GLIMMERWORKS_ECHOCOMMAND_H
#include "core/console/Command.h"


namespace glimmer {
    /**
     * Echo
     * 输出内容
     *
     * Echo [Text Content]
     * 格式为：Echo [文本内容]
     */
    class EchoCommand: public Command {
    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        explicit EchoCommand(AppContext *appContext);

        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}


#endif //GLIMMERWORKS_ECHOCOMMAND_H
