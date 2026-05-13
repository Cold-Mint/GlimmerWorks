//
// Created by coldmint on 2026/3/29.
//

#include "ScreenshotCommand.h"

#include "core/scene/AppContext.h"
#include "fmt/xchar.h"

void glimmer::ScreenshotCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("create");
}

glimmer::ScreenshotCommand::ScreenshotCommand(AppContext *appContext)
    : Command(appContext) {
}

std::string glimmer::ScreenshotCommand::GetName() const {
    return SCREEN_SHOT_COMMAND_NAME;
}

void glimmer::ScreenshotCommand::
PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[create:string]");
}

bool glimmer::ScreenshotCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                         const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const std::string type = commandArgs->AsString(1);
    if (type != "create") {
        return false;
    }
    appContext_->AddMainThreadTask([this, onMessage] {
        appContext_->CreateScreenshot(onMessage);
    });
    return true;
}
