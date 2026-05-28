//
// Created by Cold-Mint on 2026/3/29.
//

#include "EchoCommand.h"

#include "core/LangsResources.h"
#include "fmt/xchar.h"
#include "../../scene/AppContext.h"


void glimmer::EchoCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
}

glimmer::EchoCommand::EchoCommand(AppContext *appContext) : Command(appContext) {
}

std::string glimmer::EchoCommand::GetName() const {
    return ECHO_COMMAND_NAME;
}

void glimmer::EchoCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("[text:string...]");
}

bool glimmer::EchoCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                   const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    const int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::stringstream stringStream;
    for (int i = 1; i < size; ++i) {
        if (i > 0) {
            stringStream << " ";
        }
        stringStream << commandArgs->AsString(i);
    }
    onMessageRef(stringStream.str());
    return true;
}
