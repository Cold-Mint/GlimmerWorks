//
// Created by coldmint on 2026/5/1.
//

#include "HookCommand.h"
#include "../../scene/AppContext.h"
#include "core/utils/ScanCodeUtils.h"
#include "fmt/color.h"

glimmer::HookCommand::HookCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::HookCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("add")->AddChild(SCAN_KEY_DYNAMIC_SUGGESTIONS_NAME)->AddChild(
        COMMAND_HOOK_SCOPE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("remove")->AddChild(COMMAND_HOOK_ID_SUGGESTION_NAME);
    suggestionsTree.AddChild("list")->AddChild(SCAN_KEY_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::HookCommand::GetName() const {
    return HOOK_COMMAND_NAME;
}

void glimmer::HookCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[operation_type:string]");
    if (commandArgs.GetSize() >= 1) {
        const std::string operationType = commandArgs.AsString(1);
        if (operationType == "add") {
            strings.emplace_back("[scan_code:string]");
            strings.emplace_back("[scope:string]");
            strings.emplace_back("[command:string...]");
        } else if (operationType == "remove") {
            strings.emplace_back("[hook_id:int]");
        } else if (operationType == "list") {
            strings.emplace_back("[scan_code:string]");
        }
    }
}

bool glimmer::HookCommand::Execute(const CommandArgs commandArgs,
                                   std::function<void(const std::string &text)> onMessage) {
    if (appContext_ == nullptr) {
        return false;
    }
    CommandHookManager *commandHookManager = appContext_->GetCommandHookManager();
    if (commandHookManager == nullptr) {
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    const size_t size = commandArgs.GetSize();
    if (size < 2) {
        onMessage(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const std::string operationType = commandArgs.AsString(1);
    if (operationType == "add") {
        if (size < 4) {
            onMessage(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            return false;
        }
        const SDL_Scancode scancode = ScanCodeUtils::StringToScanCode(commandArgs.AsString(2));
        if (scancode == SDL_SCANCODE_UNKNOWN) {
            onMessage(langsResources->scancodeUnknown);
            return false;
        }
        const std::string scopeStr = commandArgs.AsString(3);
        auto scope = CommandHookScope::SESSION;
        std::stringstream commandStream;
        for (int i = 4; i < size; ++i) {
            if (i > 4) {
                commandStream << " ";
            }
            commandStream << commandArgs.AsString(i);
        }
        std::unique_ptr<CommandHookEntry> commandHookEntry = commandHookManager->CreateCommandHookEntry(
            scope, scancode, commandStream.str());
        if (commandHookEntry == nullptr) {
            onMessage(langsResources->hookCreateDuplicate);
            return false;
        }
        auto hookId = commandHookManager->Register(std::move(commandHookEntry));
        if (!hookId.has_value()) {
            onMessage(langsResources->hookAddDuplicate);
            return false;
        }
        onMessage(fmt::format(
            fmt::runtime(langsResources->hookAddSuccess),
            hookId.value()));
        return true;
    }
    if (operationType == "remove") {
        auto hookId = commandArgs.AsString(2);
        if (commandHookManager->Unregister(hookId)) {
            onMessage(langsResources->hookRemoveSuccess);
            return true;
        }
        onMessage(fmt::format(
            fmt::runtime(langsResources->hookIdNotExist),
            hookId));
        return false;
    }
    if (operationType == "list") {
        const SDL_Scancode scancode = ScanCodeUtils::StringToScanCode(commandArgs.AsString(2));
        if (scancode == SDL_SCANCODE_UNKNOWN) {
            onMessage(langsResources->scancodeUnknown);
            return false;
        }
        const std::vector<CommandHookEntry *> &vector = commandHookManager->GetCommandHookVector(scancode);
        if (vector.empty()) {
            onMessage(fmt::format(fmt::runtime(langsResources->scancodeHookNotFound), static_cast<uint16_t>(scancode)));
            return false;
        }
        std::stringstream stringStream;
        stringStream << fmt::format(
            fmt::runtime(langsResources->scancodeHookFoundCount),
            vector.size(), static_cast<uint16_t>(scancode));
        for (int i = 0; i < vector.size(); ++i) {
            CommandHookEntry *commandHookEntry = vector.at(i);
            if (commandHookEntry == nullptr) {
                continue;
            }
            stringStream << '\n';
            stringStream << fmt::format(fmt::runtime(langsResources->hookInfo), commandHookEntry->hookId,
                                        static_cast<uint8_t>(commandHookEntry->scope), commandHookEntry->command,
                                        static_cast<uint16_t>(commandHookEntry->scancode));
        }
        onMessage(stringStream.str());
        return true;
    }
    return false;
}
