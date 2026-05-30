/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "CommandHookManager.h"

#include "utils/EventTypeUtils.h"
#include "utils/ScanCodeUtils.h"
#include "utils/UUIDUtils.h"


bool glimmer::CommandHookManager::Exist(const CommandHookScope scope, const uint32_t key,
                                        const std::string& command) const
{
    auto iterator = sessionCommandHookMap_.find(key);
    if (iterator == sessionCommandHookMap_.end())
    {
        return false;
    }
    auto& vector = iterator->second;
    for (auto& commandHook : vector)
    {
        if (commandHook == nullptr)
        {
            continue;
        }
        if (commandHook->command == command && commandHook->scope == scope)
        {
            return true;
        }
    }
    return false;
}

std::optional<std::string> glimmer::CommandHookManager::RegisterImpl(
    std::unordered_map<uint32_t, std::vector<CommandHookEntry*>>& commandHookMap,
    std::vector<std::unique_ptr<CommandHookEntry>>& commandHookVector, CommandHookScope exclude,
    std::unique_ptr<CommandHookEntry> commandHookEntry) const
{
    if (commandHookEntry == nullptr)
    {
        return std::nullopt;
    }
    if (commandHookEntry->scope == exclude)
    {
        //Manual registration of configuration scope hooks is not allowed.
        //不能手动注册配置作用域钩子。
        return std::nullopt;
    }
    const CommandHookEntry* commandHookEntryPtr = commandHookEntry.get();

    if (Exist(commandHookEntryPtr->scope, commandHookEntryPtr->code, commandHookEntryPtr->command))
    {
        return std::nullopt;
    }
    std::string hookId = commandHookEntryPtr->hookId;
    const uint32_t key = CommandHookEntry::GetKey(commandHookEntry->eventType, commandHookEntry->code);
    CommandHookEntry* rawPtr = commandHookEntry.get();
    commandHookMap[key].push_back(rawPtr);
    commandHookVector.push_back(std::move(commandHookEntry));
    return hookId;
}

bool glimmer::CommandHookManager::UnregisterImpl(
    std::unordered_map<uint32_t, std::vector<CommandHookEntry*>>& commandHookMap,
    std::vector<std::unique_ptr<CommandHookEntry>>& commandHookVector, CommandHookScope exclude,
    const std::string& commandHookId)
{
    for (auto iter = commandHookVector.begin(); iter != commandHookVector.end(); ++iter)
    {
        auto& hookEntry = *iter;
        if (hookEntry == nullptr)
        {
            continue;
        }
        if (hookEntry->scope == exclude)
        {
            continue;
        }
        if (hookEntry->hookId == commandHookId)
        {
            const CommandHookEntry* rawPtr = hookEntry.get();
            const auto mapIter = commandHookMap.find(
                CommandHookEntry::GetKey(hookEntry->eventType, hookEntry->code));
            if (mapIter != commandHookMap.end())
            {
                auto& hookPtrVector = mapIter->second;

                for (auto ptrIter = hookPtrVector.begin(); ptrIter != hookPtrVector.end();)
                {
                    if (*ptrIter == rawPtr)
                    {
                        hookPtrVector.erase(ptrIter);
                        break;
                    }
                }
                if (hookPtrVector.empty())
                {
                    commandHookMap.erase(mapIter);
                }
            }
            commandHookVector.erase(iter);
            return true;
        }
    }
    return false;
}

glimmer::CommandHookManager::CommandHookManager(Config* config)
{
    config_ = config;
    configChangedId_ = config_->RegisterOnConfigChanged(true,
                                                        std::make_unique<std::function<void(const Config*)>>(
                                                            [this](const Config* cfg)
                                                            {
                                                                LoadHookFromConfig(cfg->commandHooks);
                                                            }));
}


glimmer::CommandHookManager::~CommandHookManager()
{
    if (config_ != nullptr && configChangedId_ != INVALID_CONFIG_CALL_BACK)
    {
        config_->UnregisterOnConfigChanged(configChangedId_);
    }
}


const std::vector<glimmer::CommandHookEntry*>& glimmer::CommandHookManager::GetCommandHookVector(const uint32_t key)
{
    fullVector_.clear();
    const auto configIterator = configCommandHookMap_.find(key);
    if (configIterator != configCommandHookMap_.end())
    {
        fullVector_.insert(fullVector_.end(),
                           configIterator->second.begin(),
                           configIterator->second.end());
    }

    const auto sessionIterator = sessionCommandHookMap_.find(key);
    if (sessionIterator != sessionCommandHookMap_.end())
    {
        fullVector_.insert(fullVector_.end(),
                           sessionIterator->second.begin(),
                           sessionIterator->second.end());
    }

    return fullVector_;
}

std::unique_ptr<glimmer::CommandHookEntry> glimmer::CommandHookManager::CreateCommandHookEntry(CommandHookScope scope,
    SDL_EventType eventType, uint16_t code, const std::string& command, bool keyRepeat) const
{
    if (scope == CommandHookScope::CONFIG)
    {
        //Cannot create configuration scope hooks.
        //不能创建配置作用域钩子。
        return nullptr;
    }
    if (Exist(scope, code, command))
    {
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

void glimmer::CommandHookManager::LoadHookFromConfig(const std::vector<CommandHookResource>& commandHooks)
{
    configCommandHookVector_.clear();
    configCommandHookMap_.clear();
    if (commandHooks.empty())
    {
        return;
    }
    for (auto& commandHookRes : commandHooks)
    {
        auto commandHookEntry = std::make_unique<CommandHookEntry>();
        commandHookEntry->hookId = UUIDUtils::Generate();
        commandHookEntry->command = commandHookRes.command;
        commandHookEntry->eventType = EventTypeUtils::StringToEventType(commandHookRes.eventType);
        commandHookEntry->keyRepeat = commandHookRes.keyRepeat;
        if (commandHookEntry->eventType == SDL_EVENT_KEY_UP || commandHookEntry->eventType == SDL_EVENT_KEY_DOWN)
        {
            commandHookEntry->code = ScanCodeUtils::StringToScanCode(commandHookRes.code);
        }
        commandHookEntry->scope = CommandHookScope::CONFIG;
        (void)RegisterImpl(configCommandHookMap_, configCommandHookVector_, CommandHookScope::SESSION,
                           std::move(commandHookEntry));
    }
}

bool glimmer::CommandHookManager::Contains(const std::string& hookId) const
{
    for (auto& sessionCommandHookVector : sessionCommandHookVector_)
    {
        if (sessionCommandHookVector == nullptr)
        {
            continue;
        }
        if (sessionCommandHookVector->hookId == hookId)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> glimmer::CommandHookManager::GetCommandHookIdsWithOutConfig() const
{
    std::vector<std::string> result;
    for (auto& sessionCommandHookVector : sessionCommandHookVector_)
    {
        result.push_back(sessionCommandHookVector->hookId);
    }
    return result;
}


std::optional<std::string> glimmer::CommandHookManager::Register(std::unique_ptr<CommandHookEntry> commandHookEntry)
{
    return RegisterImpl(sessionCommandHookMap_, sessionCommandHookVector_, CommandHookScope::CONFIG,
                        std::move(commandHookEntry));
}

bool glimmer::CommandHookManager::Unregister(const std::string& commandHookId)
{
    return UnregisterImpl(sessionCommandHookMap_, sessionCommandHookVector_, CommandHookScope::CONFIG, commandHookId);
}
