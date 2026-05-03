//
// Created by coldmint on 2026/3/29.
//

#include "ScreenshotCommand.h"

#include "core/scene/AppContext.h"
#include "fmt/xchar.h"

glimmer::ScreenshotCommand::ScreenshotCommand(AppContext *appContext)
    : Command(appContext) {
}

void glimmer::ScreenshotCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("create");
}

std::string glimmer::ScreenshotCommand::GetName() const {
    return SCREEN_SHOT_COMMAND_NAME;
}

void glimmer::ScreenshotCommand::
PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[create:string]");
}

bool glimmer::ScreenshotCommand::Execute(const CommandSender *commandSender, const CommandArgs commandArgs,
                                         std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    int size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const std::string type = commandArgs.AsString(1);
    if (type != "create") {
        return false;
    }
    appContext_->AddMainThreadTask([this,onMessage] {
        appContext_->CreateScreenshot(onMessage);
    });
    return true;
}
