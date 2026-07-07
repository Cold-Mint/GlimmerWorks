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
#include "ConfigCommand.h"

#include "core/Config.h"
#include "core/Constants.h"
#include "core/scene/AppContext.h"
#include "fmt/color.h"
#include "toml.hpp"


void glimmer::ConfigCommand::InitSuggestions(NodeTree<std::string>* suggestionsTree)
{
    if (suggestionsTree == nullptr)
    {
        return;
    }
    suggestionsTree->AddChild("get")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("set")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("commit");
}

glimmer::ConfigCommand::ConfigCommand(AppContext* appContext, toml::value* value) : Command(appContext),
    configValue_(value)
{
}

const std::string& glimmer::ConfigCommand::GetName() const
{
    return CONFIG_COMMAND_NAME;
}

void glimmer::ConfigCommand::PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings)
{
    if (strings == nullptr || commandArgs == nullptr)
    {
        return;
    }
    strings->emplace_back("[operation type:string]");
    const int size = commandArgs->GetSize();
    if (size >= 2)
    {
        std::string parameter1 = commandArgs->AsString(1);
        if (parameter1 == "get" || parameter1 == "set")
        {
            strings->emplace_back("[parameter name:string]");
        }
    }
    if (size >= 3 && commandArgs->AsString(1) == "set")
    {
        using enum ConfigType;
        if (const ConfigType configType = GetParameterType(commandArgs->AsString(2));
            configType == TYPE_BOOLEAN)
        {
            strings->emplace_back("[value:bool]");
        }
        else if (configType == TYPE_FLOAT)
        {
            strings->emplace_back("[value:float]");
        }
        else if (configType == TYPE_INT)
        {
            strings->emplace_back("[value:int]");
        }
        else if (configType == TYPE_ARRAY)
        {
            strings->emplace_back("[value:array(format:value1,value2)]");
        }
        else if (configType == TYPE_TABLE)
        {
            strings->emplace_back("[value:table]");
        }
        else
        {
            strings->emplace_back("[value:string]");
        }
    }
}

void glimmer::ConfigCommand::UpdateSetSuggestions(const CommandArgs* commandArgs)
{
    if (const auto obj = GetPrivateSuggestionsTree().GetChildByValue("set")->GetChildByValue(
        CONFIG_DYNAMIC_SUGGESTIONS_NAME); obj != nullptr)
    {
        obj->ClearChildren();
        const std::string arg2 = commandArgs->AsString(2);
        if (GetParameterType(arg2) == ConfigType::TYPE_BOOLEAN)
        {
            obj->AddChild(BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME);
        }
        if (GetParameterType(arg2) == ConfigType::TYPE_STRING && (
            arg2 == "mods.resourcePackPath" || arg2 ==
            "mods.dataPackPath"))
        {
            obj->AddChild(std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs->AsString(3));
        }
    }
}

glimmer::NodeTree<std::string>* glimmer::ConfigCommand::GetSuggestionsTree(const CommandArgs* commandArgs)
{
    const int size = commandArgs->GetSize();
    if (size > 2 && commandArgs->AsString(1) == "set")
    {
        UpdateSetSuggestions(commandArgs);
    }
    return &GetPrivateSuggestionsTree();
}


bool glimmer::ConfigCommand::Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                                     const std::function<void(const std::string& text)>* onMessage)
{
    const AppContext* appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr)
    {
        return false;
    }
    const std::function<void(const std::string& text)>& onMessageRef = *onMessage;
    const LangsResources* langsResources = appContext->GetLangsResources();
    if (langsResources == nullptr)
    {
        return false;
    }
    int size = commandArgs->GetSize();
    if (size < 2)
    {
        onMessageRef(fmt::format(
            fmt::runtime(langsResources->insufficientParameterLength),
            2, size));
        return false;
    }

    const std::string operation = commandArgs->AsString(1);
    if (operation == "commit")
    {
        if (appContext->GetVirtualFileSystem()->WriteFile(CONFIG_FILE_NAME, toml::format(*configValue_)))
        {
            appContext->GetConfig()->LoadConfig(*configValue_);
            onMessageRef(langsResources->configurationCommitSuccess);
            return true;
        }
        onMessageRef(langsResources->configurationCommitFail);
        return false;
    }

    if (operation == "get")
    {
        if (size < 3)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                3, size));
            return false;
        }
        onMessageRef(GetValue(commandArgs->AsString(2)));
        return true;
    }
    if (operation == "set")
    {
        if (size < 4)
        {
            onMessageRef(fmt::format(
                fmt::runtime(langsResources->insufficientParameterLength),
                4, size));
            return false;
        }

        std::string parameterName = commandArgs->AsString(2);
        std::string value = commandArgs->AsString(3);
        const ConfigType configType = GetParameterType(parameterName);
        if (configType == ConfigType::TYPE_BOOLEAN && value == TOGGLE_KEY_WORD)
        {
            const std::string oldValue = GetValue(parameterName);
            if (oldValue == "true")
            {
                value = "false";
            }
            else
            {
                value = "true";
            }
        }
        if (SetValue(parameterName, value))
        {
            appContext->GetConfig()->LoadConfig(*configValue_);
            onMessageRef(fmt::format(fmt::runtime(langsResources->configurationUpdate),
                                     parameterName, value));
        }
        return true;
    }

    onMessageRef(appContext->GetLangsResources()->unknownCommandParameters);
    return false;
}

glimmer::ConfigType glimmer::ConfigCommand::GetParameterType(const std::string& parameterName) const
{
    using enum ConfigType;
    if (!configValue_ || !configValue_->is_table())
    {
        return TYPE_STRING;
    }

    const toml::value* current = configValue_;

    std::size_t start = 0;
    while (true)
    {
        const std::size_t dot = parameterName.find('.', start);
        const std::string key =
            dot == std::string::npos
                ? parameterName.substr(start)
                : parameterName.substr(start, dot - start);

        // The current node must be of type "table" in order to proceed further.
        // 当前节点必须是 table 才能继续向下
        if (!current->is_table())
        {
            return TYPE_STRING;
        }

        const auto& table = current->as_table();
        const auto it = table.find(key);
        if (it == table.end())
        {
            return TYPE_STRING;
        }

        current = &it->second;

        if (dot == std::string::npos)
        {
            break;
        }
        start = dot + 1;
    }

    if (current->is_string()) return TYPE_STRING;
    if (current->is_array()) return TYPE_ARRAY;
    if (current->is_table()) return TYPE_TABLE;
    if (current->is_floating()) return TYPE_FLOAT;
    if (current->is_integer()) return TYPE_INT;
    if (current->is_boolean()) return TYPE_BOOLEAN;

    return TYPE_STRING;
}

std::string glimmer::ConfigCommand::GetValue(const std::string& parameterName) const
{
    if (!configValue_ || !configValue_->is_table())
    {
        return "";
    }

    const toml::value* current = configValue_;

    std::size_t start = 0;
    std::size_t dot = 0;
    while ((dot = parameterName.find('.', start)) != std::string::npos)
    {
        const std::string key =
            dot == std::string::npos
                ? parameterName.substr(start)
                : parameterName.substr(start, dot - start);

        // The current node must be of type "table" in order to proceed further.
        // 当前节点必须是 table 才能继续向下
        if (!current->is_table())
        {
            return "";
        }

        const auto& table = current->as_table();
        const auto it = table.find(key);
        if (it == table.end())
        {
            return "";
        }

        current = &it->second;
        start = dot + 1;
    }
    return toml::format(current->at(parameterName.substr(start)));
}

bool glimmer::ConfigCommand::SetValue(
    const std::string& parameterName,
    const std::string& value) const
{
    if (!configValue_ || !configValue_->is_table())
    {
        return false;
    }

    toml::value* current = configValue_;
    std::size_t start = 0;
    std::size_t dot = 0;

    while ((dot = parameterName.find('.', start)) != std::string::npos)
    {
        const std::string key = parameterName.substr(start, dot - start);

        if (!current->is_table())
        {
            return false;
        }

        auto& table = current->as_table();
        auto it = table.find(key);
        if (it == table.end())
        {
            return false;
        }

        current = &it->second;
        start = dot + 1;
    }

    if (!current->is_table())
    {
        return false;
    }

    const std::string finalKey = parameterName.substr(start);
    auto& table = current->as_table();

    auto it = table.find(finalKey);
    const bool exists = it != table.end();

    if (exists && it->second.is_array())
    {
        toml::array arr;

        std::string_view content(value);
        while (!content.empty())
        {
            const auto comma = content.find(',');
            std::string_view token = content.substr(0, comma);
            arr.emplace_back(std::string(token));

            if (comma == std::string_view::npos) break;
            content.remove_prefix(comma + 1);
        }

        table[finalKey] = toml::value(arr);
        return true;
    }

    if (exists && it->second.is_boolean())
    {
        table[finalKey] = toml::value(value == "true" || value == "1");
        return true;
    }

    if (exists && it->second.is_floating())
    {
        table[finalKey] = toml::value(std::stof(value));
        return true;
    }


    if (exists && it->second.is_integer())
    {
        table[finalKey] = toml::value(std::stoi(value));
        return true;
    }

    table[finalKey] = toml::value(value);
    return true;
}
