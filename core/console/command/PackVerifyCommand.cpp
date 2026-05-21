//
// Created by coldmint on 2026/5/21.
//

#include "PackVerifyCommand.h"

#include "core/mod/PackVerifyState.h"
#include "core/scene/AppContext.h"
#include "fmt/xchar.h"

void glimmer::PackVerifyCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild(DATA_PACK_SUGGESTIONS_NAME);
}

glimmer::PackVerifyCommand::PackVerifyCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::PackVerifyCommand::
PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr) {
        return;
    }
    strings->emplace_back("[pack_id:string]");
}

bool glimmer::PackVerifyCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                         const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    Config *config = appContext_->GetConfig();
    if (config == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    if (!config->mods.enableSignVerify) {
        onMessageRef(langsResources->notEnabledSignVerify);
    }
    int size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    DataPackManager *dataPackManager = appContext_->GetDataPackManager();
    if (dataPackManager == nullptr) {
        return false;
    }
    std::string packId = commandArgs->AsString(1);
    if (!dataPackManager->Contains(packId)) {
        onMessageRef(langsResources->dataPackageCannotBeFound);
        return false;
    }
    switch (dataPackManager->GetPackVerifyState(packId)) {
        case PackVerifyState::Unsigned:
            onMessageRef(langsResources->unsignedPackage);
            break;
        case PackVerifyState::VerifiedSuccess:
            onMessageRef(langsResources->signatureVerificationSuccessful);
            break;
        case PackVerifyState::VerifiedFailed:
            onMessageRef(langsResources->signatureVerificationFailed);
            break;
    }
    return true;
}

std::string glimmer::PackVerifyCommand::GetName() const {
    return PACK_VERIFY_COMMAND_NAME;
}
