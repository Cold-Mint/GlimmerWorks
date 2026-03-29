//
// Created by coldmint on 2026/3/29.
//

#include "EchoCommand.h"

#include "core/Langs.h"
#include "fmt/xchar.h"
#include "../../scene/AppContext.h"


glimmer::EchoCommand::EchoCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::EchoCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
}

std::string glimmer::EchoCommand::GetName() const {
    return ECHO_COMMAND_NAME;
}

void glimmer::EchoCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[text:string...]");
}

bool glimmer::EchoCommand::Execute(CommandArgs commandArgs, std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    const int size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    std::stringstream stringStream;
    for (int i = 1; i < size; ++i) {
        if (i > 0) {
            stringStream << " ";
        }
        stringStream << commandArgs.AsString(i);
    }
    onMessage(stringStream.str());
    return true;
}
