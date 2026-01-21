//
// Created by Cold-Mint on 2025/12/2.
//

#include "ConfigSuggestions.h"

#include "../../Constants.h"

void glimmer::ConfigSuggestions::CollectKeys(const nlohmann::json *json, const std::string &prefix,
                                             std::vector<std::string> &out) {
    if (!json->is_object()) return;

    for (auto it = json->begin(); it != json->end(); ++it) {
        std::string fullKey = prefix.empty() ? it.key() : prefix + "." + it.key();
        // If it is a nested object, it is recursively decomposed
        // 如果是嵌套对象，递归拆解
        if (it->is_object()) {
            CollectKeys(&*it, fullKey, out);
        } else {
            out.push_back(fullKey);
        }
    }
}

bool glimmer::ConfigSuggestions::Match(const std::string keyword, std::string param) {
    std::vector<std::string> fields;
    if (json_->is_object()) {
        CollectKeys(json_, "", fields);
    }
    return std::find(fields.begin(), fields.end(), keyword) != fields.end();
}

std::string glimmer::ConfigSuggestions::GetId() const {
    return CONFIG_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::ConfigSuggestions::GetSuggestions(std::string param) {
    std::vector<std::string> fields;
    if (json_->is_object()) {
        CollectKeys(json_, "", fields);
    }
    return fields;
}
