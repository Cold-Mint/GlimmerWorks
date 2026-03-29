//
// Created by coldmint on 2026/3/29.
//

#ifndef GLIMMERWORKS_SCREENSHOTCOMMAND_H
#define GLIMMERWORKS_SCREENSHOTCOMMAND_H
#include "core/console/Command.h"


namespace glimmer {
    class ScreenshotCommand : public Command{
    public:
        explicit ScreenshotCommand(AppContext *appContext);

    protected:
        void InitSuggestions(NodeTree<std::string> &suggestionsTree) override;

    public:
        [[nodiscard]] std::string GetName() const override;

        void PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) override;

        bool Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) override;
    };
}


#endif //GLIMMERWORKS_SCREENSHOTCOMMAND_H
