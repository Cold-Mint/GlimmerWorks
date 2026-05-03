//
// Created by coldmint on 2026/5/1.
//

#include "CommandHookManager.h"

#include "utils/EventTypeUtils.h"
#include "utils/ScanCodeUtils.h"
#include "utils/UUIDUtils.h"


bool glimmer::CommandHookManager::Exist(const CommandHookScope scope, const uint32_t key,
                                        const std::string &command) const {
    auto iterator = sessionCommandHookMap_.find(key);
    if (iterator == sessionCommandHookMap_.end()) {
        return false;
    }
    auto &vector = iterator->second;
    for (auto &commandHook: vector) {
        if (commandHook == nullptr) {
            continue;
        }
        if (commandHook->command == command && commandHook->scope == scope) {
            return true;
        }
    }
    return false;
}

std::optional<std::string> glimmer::CommandHookManager::RegisterImpl(
    std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > &commandHookMap,
    std::vector<std::unique_ptr<CommandHookEntry> > &commandHookVector, CommandHookScope exclude,
    std::unique_ptr<CommandHookEntry> commandHookEntry) const {
    if (commandHookEntry == nullptr) {
        return std::nullopt;
    }
    if (commandHookEntry->scope == exclude) {
        //Manual registration of configuration scope hooks is not allowed.
        //不能手动注册配置作用域钩子。
        return std::nullopt;
    }
    const CommandHookEntry *commandHookEntryPtr = commandHookEntry.get();

    if (Exist(commandHookEntryPtr->scope, commandHookEntryPtr->code, commandHookEntryPtr->command)) {
        return std::nullopt;
    }
    std::string hookId = commandHookEntryPtr->hookId;
    const uint32_t key = CommandHookEntry::GetKey(commandHookEntry->eventType, commandHookEntry->code);
    CommandHookEntry *rawPtr = commandHookEntry.get();
    commandHookMap[key].push_back(rawPtr);
    commandHookVector.push_back(std::move(commandHookEntry));
    return hookId;
}

bool glimmer::CommandHookManager::UnregisterImpl(
    std::unordered_map<uint32_t, std::vector<CommandHookEntry *> > &commandHookMap,
    std::vector<std::unique_ptr<CommandHookEntry> > &commandHookVector, CommandHookScope exclude,
    const std::string &commandHookId) {
    for (auto iter = commandHookVector.begin(); iter != commandHookVector.end(); ++iter) {
        auto &hookEntry = *iter;
        if (hookEntry == nullptr) {
            continue;
        }
        if (hookEntry->scope == exclude) {
            continue;
        }
        if (hookEntry->hookId == commandHookId) {
            const CommandHookEntry *rawPtr = hookEntry.get();
            const auto mapIter = commandHookMap.find(
                CommandHookEntry::GetKey(hookEntry->eventType, hookEntry->code));
            if (mapIter != commandHookMap.end()) {
                auto &hookPtrVector = mapIter->second;

                for (auto ptrIter = hookPtrVector.begin(); ptrIter != hookPtrVector.end();) {
                    if (*ptrIter == rawPtr) {
                        hookPtrVector.erase(ptrIter);
                        break;
                    }
                }
                if (hookPtrVector.empty()) {
                    commandHookMap.erase(mapIter);
                }
            }
            commandHookVector.erase(iter);
            return true;
        }
    }
    return false;
}


glimmer::CommandHookManager::CommandHookManager() = default;

const std::vector<glimmer::CommandHookEntry *> &glimmer::CommandHookManager::GetCommandHookVector(const uint32_t key) {
    fullVector_.clear();
    const auto configIterator = configCommandHookMap_.find(key);
    if (configIterator != configCommandHookMap_.end()) {
        fullVector_.insert(fullVector_.end(),
                           configIterator->second.begin(),
                           configIterator->second.end());
    }

    const auto sessionIterator = sessionCommandHookMap_.find(key);
    if (sessionIterator != sessionCommandHookMap_.end()) {
        fullVector_.insert(fullVector_.end(),
                           sessionIterator->second.begin(),
                           sessionIterator->second.end());
    }

    return fullVector_;
}

std::unique_ptr<glimmer::CommandHookEntry> glimmer::CommandHookManager::CreateCommandHookEntry(CommandHookScope scope,
    SDL_EventType eventType, uint16_t code, const std::string &command, bool keyRepeat) const {
    if (scope == CommandHookScope::CONFIG) {
        //Cannot create configuration scope hooks.
        //不能创建配置作用域钩子。
        return nullptr;
    }
    if (Exist(scope, code, command)) {
        return nullptr;
    }
    auto commandHookEntry = std::make_unique<CommandHookEntry>();
    commandHookEntry->hookId = UUIDUtils::Generate();
    commandHookEntry->scope = scope;
    commandHookEntry->keyRepeat = keyRepeat;
    commandHookEntry->eventType = eventType;
    commandHookEntry->command = command;
    commandHookEntry->code = code;
    return commandHookEntry;
}

void glimmer::CommandHookManager::LoadHookFromConfig(const std::vector<CommandHookResource> &commandHooks) {
    configCommandHookVector_.clear();
    configCommandHookMap_.clear();
    if (commandHooks.empty()) {
        return;
    }
    for (auto &commandHookRes: commandHooks) {
        auto commandHookEntry = std::make_unique<CommandHookEntry>();
        commandHookEntry->hookId = UUIDUtils::Generate();
        commandHookEntry->command = commandHookRes.command;
        commandHookEntry->eventType = EventTypeUtils::StringToEventType(commandHookRes.eventType);
        commandHookEntry->keyRepeat = commandHookRes.keyRepeat;
        if (commandHookEntry->eventType == SDL_EVENT_KEY_UP || commandHookEntry->eventType == SDL_EVENT_KEY_DOWN) {
            commandHookEntry->code = ScanCodeUtils::StringToScanCode(commandHookRes.code);
        }
        commandHookEntry->scope = CommandHookScope::CONFIG;
        (void) RegisterImpl(configCommandHookMap_, configCommandHookVector_, CommandHookScope::SESSION,
                            std::move(commandHookEntry));
    }
}

std::vector<std::string> glimmer::CommandHookManager::GetCommandHookIdsWithOutConfig() const {
    std::vector<std::string> result;
    for (auto &sessionCommandHookVector: sessionCommandHookVector_) {
        result.push_back(sessionCommandHookVector->hookId);
    }
    return result;
}


std::optional<std::string> glimmer::CommandHookManager::Register(std::unique_ptr<CommandHookEntry> commandHookEntry) {
    return RegisterImpl(sessionCommandHookMap_, sessionCommandHookVector_, CommandHookScope::CONFIG,
                        std::move(commandHookEntry));
}

bool glimmer::CommandHookManager::Unregister(const std::string &commandHookId) {
    return UnregisterImpl(sessionCommandHookMap_, sessionCommandHookVector_, CommandHookScope::CONFIG, commandHookId);
}
