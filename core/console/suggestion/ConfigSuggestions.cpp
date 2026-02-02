//
// Created by Cold-Mint on 2025/12/2.
//

#include "ConfigSuggestions.h"

#include "../../Constants.h"

void glimmer::ConfigSuggestions::ParseTable(const toml::value::table_type &table, std::vector<std::string> &fields,
                                            const std::string &prefix) {
    for (const auto &[key, val]: table) {
        std::string fullKey;
        if (prefix.empty()) {
            fullKey = key;
        } else {
            fullKey.reserve(prefix.size() + 1 + key.size());
            fullKey = prefix;
            fullKey.push_back('.');
            fullKey += key;
        }
        if (val.is_table()) {
            ParseTable(val.as_table(), fields, fullKey);
        } else {
            fields.push_back(fullKey);
        }
    }
}

glimmer::ConfigSuggestions::ConfigSuggestions(toml::value *configValue) : configValue_(configValue) {
}

bool glimmer::ConfigSuggestions::Match(const std::string keyword, std::string param) {
    std::vector<std::string> fields;
    ParseTable(configValue_->as_table(), fields);
    return std::find(fields.begin(), fields.end(), keyword) != fields.end();
}

std::string glimmer::ConfigSuggestions::GetId() const {
    return CONFIG_DYNAMIC_SUGGESTIONS_NAME;
}

std::vector<std::string> glimmer::ConfigSuggestions::GetSuggestions(std::string param) {
    std::vector<std::string> fields;
    ParseTable(configValue_->as_table(), fields);
    return fields;
}
