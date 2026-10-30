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

#include "core/config/Config.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "toml.hpp"


void glimmer::ConfigCommand::InitSuggestions(NodeTree<std::string> *suggestionsTree) {
    if (suggestionsTree == nullptr) {
        return;
    }
    suggestionsTree->AddChild("get")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("set")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree->AddChild("commit");
}

const std::string &glimmer::ConfigCommand::GetName() const {
    return CONFIG_COMMAND_NAME;
}

void glimmer::ConfigCommand::PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) {
    if (strings == nullptr || commandArgs == nullptr) {
        return;
    }
    strings->emplace_back("[operation type:string]");
    const int size = commandArgs->GetSize();
    if (size >= 2) {
        std::string parameter1 = commandArgs->AsString(1);
        if (parameter1 == "get" || parameter1 == "set") {
            strings->emplace_back("[parameter name:string]");
        }
    }
    if (size >= 3 && commandArgs->AsString(1) == "set") {
        using enum ConfigType;
        if (const ConfigType configType = GetParameterType(commandArgs->AsString(2));
            configType == TYPE_BOOLEAN) {
            strings->emplace_back("[value:bool]");
        } else if (configType == TYPE_FLOAT) {
            strings->emplace_back("[value:float]");
        } else if (configType == TYPE_INT) {
            strings->emplace_back("[value:int]");
        } else if (configType == TYPE_ARRAY) {
            strings->emplace_back("[value:array(format:value1,value2)]");
        } else {
            strings->emplace_back("[value:string]");
        }
    }
}

void glimmer::ConfigCommand::UpdateSetSuggestions(const CommandArgs *commandArgs) {
    if (const auto obj = GetPrivateSuggestionsTree().GetChildByValue("set")->GetChildByValue(
        CONFIG_DYNAMIC_SUGGESTIONS_NAME); obj != nullptr) {
        obj->ClearChildren();
        const std::string arg2 = commandArgs->AsString(2);
        if (GetParameterType(arg2) == ConfigType::TYPE_BOOLEAN) {
            obj->AddChild(BOOL_TOGGLE_DYNAMIC_SUGGESTIONS_NAME);
        }
        if (GetParameterType(arg2) == ConfigType::TYPE_STRING && (
                arg2 == "resource_pack_path" || arg2 ==
                "data_pack_path")) {
            obj->AddChild(std::string(VFS_DYNAMIC_SUGGESTIONS_NAME) + ":" + commandArgs->AsString(3));
        }
    }
}

glimmer::ConfigCommand::ConfigCommand(AppContext *appContext) : Command(appContext),
                                                                config_(appContext->GetConfig()),
                                                                virtualFileSystem_(appContext->GetVirtualFileSystem()) {
}

glimmer::NodeTree<std::string> *glimmer::ConfigCommand::GetSuggestionsTree(const CommandArgs *commandArgs) {
    const int size = commandArgs->GetSize();
    if (size > 2 && commandArgs->AsString(1) == "set") {
        UpdateSetSuggestions(commandArgs);
    }
    return &GetPrivateSuggestionsTree();
}


bool glimmer::ConfigCommand::ExecuteCommit(const VirtualFileSystem *virtualFileSystem, Config *config,
                                           const LangsResources *langsResources,
                                           const std::function<void(const std::string &text)> &onMessageRef) {
    if (virtualFileSystem == nullptr || config == nullptr) {
        return false;
    }
    if (virtualFileSystem->WriteFile(CONFIG_FILE_NAME, toml::format(config->Save()))) {
        onMessageRef(langsResources->configurationCommitSuccess);
        return true;
    }
    onMessageRef(langsResources->configurationCommitFail);
    return false;
}


bool glimmer::ConfigCommand::Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                                     const std::function<void(const std::string &text)> *onMessage) {
    const AppContext *appContext = GetAppContext();
    if (appContext == nullptr || commandArgs == nullptr || onMessage == nullptr) {
        return false;
    }
    const std::function<void(const std::string &text)> &onMessageRef = *onMessage;
    const LangsResources *langsResources = appContext->GetLangsResources();
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

    const std::string operation = commandArgs->AsString(1);
    if (operation == "commit") {
        return ExecuteCommit(virtualFileSystem_, config_, langsResources, onMessageRef);
    }
    if (operation == "get") {
        return ExecuteGet(size, commandArgs, langsResources, this, onMessageRef);
    }
    if (operation == "set") {
        return ExecuteSet(size, commandArgs, langsResources, this, onMessageRef);
    }

    onMessageRef(appContext->GetLangsResources()->unknownCommandParameters);
    return false;
}

glimmer::ConfigType glimmer::ConfigCommand::GetParameterType(const std::string &parameterName) const {
    if (config_ == nullptr) {
        return ConfigType::TYPE_STRING;
    }
    const SettingDef *setting = config_->Find(parameterName);
    if (setting == nullptr) {
        return ConfigType::TYPE_STRING;
    }
    switch (setting->type) {
        case SettingType::Bool:
            return ConfigType::TYPE_BOOLEAN;
        case SettingType::Int:
        case SettingType::IntRange:
            return ConfigType::TYPE_INT;
        case SettingType::Float:
        case SettingType::FloatRange:
            return ConfigType::TYPE_FLOAT;
        case SettingType::List:
            return ConfigType::TYPE_ARRAY;
        case SettingType::String:
        case SettingType::Enum:
        default:
            return ConfigType::TYPE_STRING;
    }
}

std::string glimmer::ConfigCommand::GetValue(const std::string &parameterName) const {
    if (config_ == nullptr) {
        return "";
    }
    return config_->GetAsString(parameterName);
}

bool glimmer::ConfigCommand::SetValue(
    const std::string &parameterName,
    const std::string &value) const {
    if (config_ == nullptr) {
        return false;
    }
    return config_->SetFromString(parameterName, value);
}
