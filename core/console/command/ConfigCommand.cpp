//
// Created by Cold-Mint on 2025/12/2.
//

#include "ConfigCommand.h"

#include "../../Config.h"
#include "../../Constants.h"
#include "../../scene/AppContext.h"


void glimmer::ConfigCommand::InitSuggestions(NodeTree<std::string> &suggestionsTree) {
    suggestionsTree.AddChild("get")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
    suggestionsTree.AddChild("set")->AddChild(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
}

std::string glimmer::ConfigCommand::GetName() const {
    return CONFIG_COMMAND_NAME;
}

void glimmer::ConfigCommand::PutCommandStructure(const CommandArgs &commandArgs, std::vector<std::string> &strings) {
    strings.emplace_back("[operation type:string]");
    strings.emplace_back("[parameter name:string]");
    if (commandArgs.GetSize() > 2) {
        std::string operation = commandArgs.AsString(1);
        if (operation == "set") {
            std::string parameterName = commandArgs.AsString(2);
            ConfigType configType = GetParameterType(parameterName);
            if (configType == Boolean) {
                strings.emplace_back("[value:bool]");
            } else if (configType == Number) {
                strings.emplace_back("[value:number]");
            } else if (configType == Array) {
                strings.emplace_back("[value:array]");
            } else if (configType == Object) {
                strings.emplace_back("[value:object]");
            } else {
                strings.emplace_back("[value:string]");
            }
        }
    }
}

glimmer::ConfigType glimmer::ConfigCommand::GetParameterType(const std::string &parameterName) const {
    std::vector<std::string> parts;
    std::istringstream ss(parameterName);
    std::string token;
    while (std::getline(ss, token, '.')) {
        parts.push_back(token);
    }

    nlohmann::json current = json_;
    for (size_t i = 0; i < parts.size(); ++i) {
        const std::string &part = parts[i];
        if (current.is_object()) {
            auto it = current.find(part); // 严格匹配
            if (it != current.end()) {
                if (i == parts.size() - 1) {
                    // 最后一级，输出类型
                    if (it->is_string()) {
                        return String;
                    }
                    if (it->is_number()) {
                        return Number;
                    }
                    if (it->is_boolean()) {
                        return Boolean;
                    }
                    if (it->is_object()) {
                        return Object;
                    }
                    if (it->is_array()) {
                        return Array;
                    }
                } else {
                    current = *it; // 进入下一层
                }
            } else {
                return String;
            }
        } else {
            // 到非对象层，直接提示 string
            return String;
        }
    }

    return String;
}

bool FindOrCreateJsonNode(const std::string &path,
                          nlohmann::json &root,
                          nlohmann::json *&outObject,
                          std::string &finalKey) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '.')) {
        if (!token.empty()) parts.push_back(token);
    }

    if (parts.empty()) return false;

    nlohmann::json *current = &root;

    // 遍历到倒数第二级
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        auto &key = parts[i];

        if (!(*current)[key].is_object()) {
            (*current)[key] = nlohmann::json::object();
        }
        current = &(*current)[key];
    }

    finalKey = parts.back();
    outObject = current;
    return true;
}

bool FindJsonNode(const std::string &path,
                  const nlohmann::json &root,
                  const nlohmann::json *&out) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '.')) {
        if (!token.empty()) parts.push_back(token);
    }

    if (parts.empty()) return false;

    const nlohmann::json *current = &root;
    for (const auto &p: parts) {
        if (!current->is_object()) return false;
        if (!current->contains(p)) return false;
        current = &(*current)[p];
    }

    out = current;
    return true;
}


bool glimmer::ConfigCommand::Execute(CommandArgs commandArgs,
                                     std::function<void(const std::string &text)> onOutput) {
    if (commandArgs.GetSize() < 3) {
        return false;
    }

    std::string operation = commandArgs.AsString(1);
    if (operation == "set") {
        if (commandArgs.GetSize() < 4) {
            onOutput("Missing value for set");
            return false;
        }

        std::string parameterName = commandArgs.AsString(2);
        std::string value = commandArgs.AsString(3);

        nlohmann::json *targetObject;
        std::string lastKey;

        if (!FindOrCreateJsonNode(parameterName, json_, targetObject, lastKey)) {
            onOutput("Invalid parameter path: " + parameterName);
            return false;
        }

        // 自动推断类型
        if (appContext_->GetDynamicSuggestionsManager()->GetSuggestions(BOOL_DYNAMIC_SUGGESTIONS_NAME)->Match(value, "")) {
            (*targetObject)[lastKey] = value == "true" || value == "yes" || value == "1" || value == "y";
        } else if (std::isdigit(value[0]) || value[0] == '-' || value[0] == '+') {
            try {
                (*targetObject)[lastKey] = std::stod(value);
            } catch (...) {
                (*targetObject)[lastKey] = value;
            }
        } else {
            (*targetObject)[lastKey] = value;
        }

        onOutput("Set " + parameterName + " = " + value);
        std::string data = json_.dump();
        bool update = appContext_->GetVirtualFileSystem()->WriteFile(CONFIG_FILE_NAME, data);
        if (update) {
            appContext_->GetConfig()->LoadConfig(json_);
        }
        return update;
    }

    if (operation == "get") {
        std::string parameterName = commandArgs.AsString(2);

        const nlohmann::json *found;
        if (!FindJsonNode(parameterName, json_, found)) {
            onOutput("Config key not found: " + parameterName);
            return false;
        }

        onOutput(found->dump());
        return true;
    }

    onOutput("Unknown operation: " + operation);
    return false;
}


glimmer::NodeTree<std::string> glimmer::ConfigCommand::GetSuggestionsTree(const CommandArgs &commandArgs) {
    if (commandArgs.GetSize() > 2) {
        std::string operation = commandArgs.AsString(1);
        if (operation == "set") {
            std::string parameterName = commandArgs.AsString(2);
            ConfigType configType = GetParameterType(parameterName);
            const auto obj = suggestionsTree_.GetChildByValue("set")->GetChildByValue(CONFIG_DYNAMIC_SUGGESTIONS_NAME);
            if (obj != nullptr) {
                obj->ClearChildren();
                if (configType == Boolean) {
                    obj->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME);
                }
            }
        }
    }
    return suggestionsTree_;
}
