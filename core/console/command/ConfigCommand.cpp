//
// Created by Cold-Mint on 2025/12/2.
//

#include "ConfigCommand.h"

#include "../../Config.h"
#include "../../Constants.h"
#include "../../scene/AppContext.h"
#include "fmt/color.h"


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
        if (commandArgs.AsString(1) == "set") {
            if (const ConfigType configType = GetParameterType(commandArgs.AsString(2)); configType == Boolean) {
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

glimmer::ConfigType glimmer::ConfigCommand::GetParameterType(const std::string &parameterPath) const {
    std::vector<std::string> pathSegments;
    std::istringstream pathStream(parameterPath);
    std::string segment;

    while (std::getline(pathStream, segment, '.')) {
        pathSegments.push_back(segment);
    }
    nlohmann::json currentNode = json_;

    for (size_t index = 0; index < pathSegments.size(); ++index) {
        const std::string &key = pathSegments[index];
        if (!currentNode.is_object()) {
            return String;
        }

        auto it = currentNode.find(key);
        if (it == currentNode.end()) {
            return String;
        }
        if (index == pathSegments.size() - 1) {
            if (it->is_string()) return String;
            if (it->is_number()) return Number;
            if (it->is_boolean()) return Boolean;
            if (it->is_object()) return Object;
            if (it->is_array()) return Array;

            return String;
        }

        currentNode = *it;
    }

    return String;
}

/**
 * FindJsonNodeStrict
 * 查找Json节点
 * @param path path 路径
 * @param root Root json 根json
 * @param outObject outObject 目标对象
 * @param finalKey json键 jsonKey
 * @return
 */
bool FindJsonNodeStrict(const std::string &path,
                        nlohmann::json &root,
                        nlohmann::json *&outObject,
                        std::string &finalKey) {
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '.')) {
        if (token.empty()) {
            continue;
        }
        parts.push_back(token);
    }

    if (parts.empty()) {
        return false;
    }

    nlohmann::json *current = &root;
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        auto &key = parts[i];
        if (!current->is_object()) {
            return false;
        }
        auto it = current->find(key);
        if (it == current->end()) {
            return false;
        }
        if (!it->is_object()) {
            return false;
        }
        current = &*it;
    }
    if (const std::string &back = parts.back(); current->contains(back)) {
        finalKey = back;
        outObject = current;
        return true;
    }
    return false;
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
                                     std::function<void(const std::string &text)> onMessage) {
    int size = commandArgs.GetSize();
    if (size < 3) {
        onMessage(fmt::format(
            fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
            3, size));
        return false;
    }

    const std::string operation = commandArgs.AsString(1);
    if (operation == "set") {
        if (size < 4) {
            onMessage(fmt::format(
                fmt::runtime(appContext_->GetLangsResources()->insufficientParameterLength),
                4, size));
            return false;
        }

        std::string parameterName = commandArgs.AsString(2);
        std::string value = commandArgs.AsString(3);

        nlohmann::json *targetObject;
        std::string lastKey;

        if (!FindJsonNodeStrict(parameterName, json_, targetObject, lastKey)) {
            onMessage(fmt::format(fmt::runtime(appContext_->GetLangsResources()->entryCannotFoundInConfigurationFile),
                                  parameterName));
            return false;
        }

        // 自动推断类型
        if (appContext_->GetDynamicSuggestionsManager()->GetSuggestions(BOOL_DYNAMIC_SUGGESTIONS_NAME)->
            Match(value, "")) {
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

        onMessage(fmt::format(fmt::runtime(appContext_->GetLangsResources()->configurationUpdate),
                              parameterName, value));
        const bool update = appContext_->GetVirtualFileSystem()->WriteFile(CONFIG_FILE_NAME, json_.dump());
        if (update) {
            appContext_->GetConfig()->LoadConfig(json_);
        }
        return update;
    }

    if (operation == "get") {
        std::string parameterName = commandArgs.AsString(2);

        const nlohmann::json *found;
        if (!FindJsonNode(parameterName, json_, found)) {
            onMessage(fmt::format(fmt::runtime(appContext_->GetLangsResources()->entryCannotFoundInConfigurationFile),
                                  parameterName));
            return false;
        }

        onMessage(found->dump());
        return true;
    }
    onMessage(appContext_->GetLangsResources()->unknownCommandParameters);
    return false;
}


glimmer::NodeTree<std::string> glimmer::ConfigCommand::GetSuggestionsTree(const CommandArgs &commandArgs) {
    if (commandArgs.GetSize() > 2) {
        if (commandArgs.AsString(1) == "set") {
            if (const auto obj = suggestionsTree_.GetChildByValue("set")->GetChildByValue(
                CONFIG_DYNAMIC_SUGGESTIONS_NAME); obj != nullptr) {
                obj->ClearChildren();
                if (GetParameterType(commandArgs.AsString(2)) == Boolean) {
                    obj->AddChild(BOOL_DYNAMIC_SUGGESTIONS_NAME);
                }
            }
        }
    }
    return suggestionsTree_;
}
