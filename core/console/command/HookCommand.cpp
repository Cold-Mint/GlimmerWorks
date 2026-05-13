//
// Created by coldmint on 2026/5/1.
//

#include "HookCommand.h"
#include "../../scene/AppContext.h"
#include "core/utils/EventTypeUtils.h"
#include "core/utils/MouseButtonUtils.h"
#include "core/utils/ScanCodeUtils.h"
#include "fmt/color.h"

glimmer::HookCommand::HookCommand(AppContext *appContext) : Command(appContext) {
}

void glimmer::HookCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("add")->AddChild(
        EVENT_TYPE_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("remove")->AddChild(COMMAND_HOOK_ID_SUGGESTION_NAME);
    suggestionsTree->AddChild("list")->AddChild(EVENT_TYPE_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::HookCommand::GetName() const {
    return HOOK_COMMAND_NAME;
}

void glimmer::HookCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (commandArgs == nullptr || strings == nullptr) {
        return;
    }
    strings->emplace_back("[operation_type:string]");
    int size = commandArgs->GetSize();
    if (size > 1) {
        const std::string operationType = commandArgs->AsString(1);
        if (operationType == "add") {
            strings->emplace_back("[event_type:string]");
            if (size > 2) {
                const SDL_EventType eventType = EventTypeUtils::StringToEventType(commandArgs->AsString(2));
                if (eventType == SDL_EVENT_KEY_UP || eventType == SDL_EVENT_KEY_DOWN) {
                    strings->emplace_back("[scan_code:string]");
                    strings->emplace_back("[key_repeat:bool]");
                }
                if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
                    strings->emplace_back("[mouse_button:string]");
                }
            }
            strings->emplace_back("[scope:string]");
            strings->emplace_back("[command:string...]");
        } else if (operationType == "remove") {
            strings->emplace_back("[hook_id:int]");
        } else if (operationType == "list") {
            strings->emplace_back("[event_type:string]");
            if (size > 2) {
                const SDL_EventType eventType = EventTypeUtils::StringToEventType(commandArgs->AsString(2));
                if (eventType == SDL_EVENT_KEY_UP || eventType == SDL_EVENT_KEY_DOWN) {
                    strings->emplace_back("[scan_code:string]");
                }
                if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
                    strings->emplace_back("[mouse_button:string]");
                }
            }
        }
    }
}

glimmer::NodeTree<std::string> *glimmer::HookCommand::GetSuggestionsTree(const CommandArgs *commandArgs) {
    int size = commandArgs->GetSize();
    if (size > 2) {
        const std::string operationType = commandArgs->AsString(1);
        if (operationType == "add") {
            auto operationTypeTree = suggestionsTree_.GetChildByValue(operationType);
            if (operationTypeTree == nullptr) {
                return &suggestionsTree_;
            }
            auto eventTypeTree = operationTypeTree->GetChildByValue(EVENT_TYPE_DYNAMIC_SUGGESTIONS_NAME);
            if (eventTypeTree == nullptr) {
                return &suggestionsTree_;
            }
            const std::string eventTypeString = commandArgs->AsString(2);
            const SDL_EventType eventType = EventTypeUtils::StringToEventType(eventTypeString);
            eventTypeTree->ClearChildren();
            if (eventType == SDL_EVENT_KEY_DOWN || eventType == SDL_EVENT_KEY_UP) {
                eventTypeTree->AddChild(SCAN_KEY_DYNAMIC_SUGGESTIONS_NAME)->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME)->
                        AddChild(
                            COMMAND_HOOK_SCOPE_DYNAMIC_SUGGESTIONS_NAME);
            } else if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
                eventTypeTree->AddChild(MOUSE_BUTTON_DYNAMIC_SUGGESTIONS_NAME)->
                        AddChild(
                            COMMAND_HOOK_SCOPE_DYNAMIC_SUGGESTIONS_NAME);
            } else {
                eventTypeTree->AddChild(
                    COMMAND_HOOK_SCOPE_DYNAMIC_SUGGESTIONS_NAME);
            }
        } else if (operationType == "list") {
            auto operationTypeTree = suggestionsTree_.GetChildByValue(operationType);
            if (operationTypeTree == nullptr) {
                return &suggestionsTree_;
            }
            auto eventTypeTree = operationTypeTree->GetChildByValue(EVENT_TYPE_DYNAMIC_SUGGESTIONS_NAME);
            if (eventTypeTree == nullptr) {
                return &suggestionsTree_;
            }
            const std::string eventTypeString = commandArgs->AsString(2);
            const SDL_EventType eventType = EventTypeUtils::StringToEventType(eventTypeString);
            eventTypeTree->ClearChildren();
            if (eventType == SDL_EVENT_KEY_DOWN || eventType == SDL_EVENT_KEY_UP) {
                eventTypeTree->AddChild(SCAN_KEY_DYNAMIC_SUGGESTIONS_NAME);
            }
            if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
                eventTypeTree->AddChild(MOUSE_BUTTON_DYNAMIC_SUGGESTIONS_NAME);
            }
        }
    }
    return &suggestionsTree_;
}

bool glimmer::HookCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                   const std::function<void(const std::string &text)> *onMessage) {
    if (appContext_ == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    CommandHookManager *commandHookManager = appContext_->GetCommandHookManager();
    if (commandHookManager == nullptr) {
        return false;
    }
    const LangsResources *langsResources = appContext_->GetLangsResources();
    if (langsResources == nullptr) {
        return false;
    }
    const size_t size = commandArgs->GetSize();
    if (size < 2) {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }
    const std::string operationType = commandArgs->AsString(1);
    if (operationType == "add") {
        if (size < 4) {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            return false;
        }
        const SDL_EventType eventType = EventTypeUtils::StringToEventType(commandArgs->AsString(2));
        uint16_t code = 0;
        int startIndex = 3;
        bool keyRepeat = false;
        if (eventType == SDL_EVENT_KEY_UP || eventType == SDL_EVENT_KEY_DOWN) {
            code = static_cast<uint16_t>(ScanCodeUtils::StringToScanCode(commandArgs->AsString(startIndex)));
            startIndex++;
            if (code == SDL_SCANCODE_UNKNOWN) {
                onMessageRef(langsResources->scancodeUnknown);
                return false;
            }
            keyRepeat = commandArgs->AsBool(startIndex);
            startIndex++;
        }
        if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
            code = static_cast<uint16_t>(MouseButtonUtils::StringToMouseButton(commandArgs->AsString(3)));
            startIndex++;
        }
        const std::string scopeStr = commandArgs->AsString(startIndex);
        auto scope = CommandHookScope::SESSION;
        std::stringstream commandStream;
        startIndex++;
        for (int i = startIndex; i < size; ++i) {
            if (i > startIndex) {
                commandStream << " ";
            }
            commandStream << commandArgs->AsString(i);
        }
        std::unique_ptr<CommandHookEntry> commandHookEntry = commandHookManager->CreateCommandHookEntry(
            scope, eventType, code, commandStream.str(), keyRepeat);
        if (commandHookEntry == nullptr) {
            onMessageRef(langsResources->hookCreateDuplicate);
            return false;
        }
        auto hookId = commandHookManager->Register(std::move(commandHookEntry));
        if (!hookId.has_value()) {
            onMessageRef(langsResources->hookAddDuplicate);
            return false;
        }
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->hookAddSuccess),
            hookId.value()));
        return true;
    }
    if (operationType == "remove") {
        auto hookId = commandArgs->AsString(2);
        if (commandHookManager->Unregister(hookId)) {
            onMessageRef(langsResources->hookRemoveSuccess);
            return true;
        }
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->hookIdNotExist),
            hookId));
        return false;
    }
    if (operationType == "list") {
        const SDL_EventType eventType = EventTypeUtils::StringToEventType(commandArgs->AsString(2));
        uint16_t code = 0;
        if (eventType == SDL_EVENT_KEY_UP || eventType == SDL_EVENT_KEY_DOWN) {
            code = static_cast<uint16_t>(ScanCodeUtils::StringToScanCode(commandArgs->AsString(3)));
            if (code == SDL_SCANCODE_UNKNOWN) {
                onMessageRef(langsResources->scancodeUnknown);
                return false;
            }
        }
        if (eventType == SDL_EVENT_MOUSE_BUTTON_DOWN || eventType == SDL_EVENT_MOUSE_BUTTON_UP) {
            code = static_cast<uint16_t>(MouseButtonUtils::StringToMouseButton(commandArgs->AsString(3)));
        }
        const std::vector<CommandHookEntry *> &vector = commandHookManager->GetCommandHookVector(
            CommandHookEntry::GetKey(eventType, code));
        if (vector.empty()) {
            onMessageRef(fmt::format(fmt::runtime(langsResources->scancodeHookNotFound), code));
            return false;
        }
        std::stringstream stringStream;
        stringStream << fmt::format(
            fmt::runtime(langsResources->scancodeHookFoundCount),
            vector.size(), code);
        for (int i = 0; i < vector.size(); ++i) {
            CommandHookEntry *commandHookEntry = vector.at(i);
            if (commandHookEntry == nullptr) {
                continue;
            }
            stringStream << '\n';
            stringStream << fmt::format(fmt::runtime(langsResources->hookInfo), commandHookEntry->hookId,
                                        static_cast<uint8_t>(commandHookEntry->scope), commandHookEntry->command,
                                        code);
        }
        onMessageRef(stringStream.str());
        return true;
    }
    return false;
}
