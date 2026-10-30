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
#include "core/config/Config.h"

#include <type_traits>

#include "core/log/LogCat.h"
#include "core/utils/StringUtils.h"
#include "fmt/format.h"
#include "toml11/find.hpp"


template<>
struct toml::from<glimmer::CommandHookResource> {
    static glimmer::CommandHookResource from_toml(const value &v) {
        glimmer::CommandHookResource commandHookResource{};
        commandHookResource.hookId = toml::find<std::string>(v, "hook_id");
        commandHookResource.command = toml::find<std::string>(v, "command");
        commandHookResource.keyRepeat = toml::find_or<bool>(v, "key_repeat", false);
        commandHookResource.code = toml::find<std::string>(v, "code");
        commandHookResource.eventType = toml::find<std::string>(v, "event_type");
        return commandHookResource;
    }
};


namespace {
    glimmer::SettingType ParseSettingType(const std::string &text) {
        if (text == "bool") {
            return glimmer::SettingType::Bool;
        }
        if (text == "int") {
            return glimmer::SettingType::Int;
        }
        if (text == "float") {
            return glimmer::SettingType::Float;
        }
        if (text == "enum") {
            return glimmer::SettingType::Enum;
        }
        if (text == "int_range") {
            return glimmer::SettingType::IntRange;
        }
        if (text == "float_range") {
            return glimmer::SettingType::FloatRange;
        }
        if (text == "list") {
            return glimmer::SettingType::List;
        }
        return glimmer::SettingType::String;
    }

    std::string SettingTypeToString(glimmer::SettingType type) {
        using glimmer::SettingType;
        switch (type) {
            case SettingType::Bool: return "bool";
            case SettingType::Int: return "int";
            case SettingType::Float: return "float";
            case SettingType::Enum: return "enum";
            case SettingType::IntRange: return "int_range";
            case SettingType::FloatRange: return "float_range";
            case SettingType::List: return "list";
            default: return "string";
        }
    }

    glimmer::SettingValue ParseValue(const toml::value &table, const glimmer::SettingType type, const char *key) {
        using glimmer::SettingType;
        switch (type) {
            case SettingType::Bool:
                return toml::find_or<bool>(table, key, false);
            case SettingType::Int:
            case SettingType::IntRange:
                return toml::find_or<int>(table, key, 0);
            case SettingType::Float:
            case SettingType::FloatRange:
                return toml::find_or<float>(table, key, 0.0F);
            case SettingType::List:
                return toml::find_or<std::vector<std::string> >(table, key, {});
            case SettingType::Enum:
            case SettingType::String:
            default:
                return toml::find_or<std::string>(table, key, "");
        }
    }

    std::optional<float> ParseOptionalFloat(const toml::value &table, const char *key) {
        if (!table.contains(key)) {
            return std::nullopt;
        }
        const auto &value = table.at(key);
        if (value.is_floating()) {
            return static_cast<float>(value.as_floating());
        }
        if (value.is_integer()) {
            return static_cast<float>(value.as_integer());
        }
        return std::nullopt;
    }

    toml::value ValueToToml(const glimmer::SettingValue &value) {
        return std::visit([](const auto &v) -> toml::value {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::vector<std::string> >) {
                toml::array arr;
                for (const auto &item: v) {
                    arr.emplace_back(item);
                }
                return toml::value(arr);
            } else {
                return toml::value(v);
            }
        }, value);
    }
} // namespace


glimmer::SettingDef *glimmer::Config::Find(const std::string_view key) {
    const auto it = settingsByKey_.find(std::string(key));
    return it == settingsByKey_.end() ? nullptr : it->second;
}

const glimmer::SettingDef *glimmer::Config::Find(const std::string_view key) const {
    const auto it = settingsByKey_.find(std::string(key));
    return it == settingsByKey_.end() ? nullptr : it->second;
}

std::string glimmer::Config::GetAsString(const std::string_view key) const {
    const SettingDef *setting = Find(key);
    return setting == nullptr ? "" : setting->GetAsString();
}

bool glimmer::Config::SetFromString(const std::string_view key, const std::string &text) {
    SettingDef *setting = Find(key);
    if (setting == nullptr) {
        return false;
    }
    if (setting->SetFromString(text)) {
        fingerprint_++;
        return true;
    }
    return false;
}

std::vector<uint64_t> glimmer::Config::GetUint64List(const std::string_view key) const {
    const SettingDef *setting = Find(key);
    if (setting == nullptr) {
        return {};
    }
    const auto *list = std::get_if<std::vector<std::string> >(&setting->value);
    if (list == nullptr) {
        return {};
    }
    std::vector<uint64_t> result;
    result.reserve(list->size());
    for (const auto &item: *list) {
        result.emplace_back(StringUtils::StringToUint64(item));
    }
    return result;
}

std::string glimmer::SettingDef::GetAsString() const {
    return std::visit([](const auto &v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, float>) {
            return fmt::format("{}", v);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, std::vector<std::string> >) {
            std::string out;
            for (size_t i = 0; i < v.size(); ++i) {
                if (i != 0) {
                    out += ',';
                }
                out += v[i];
            }
            return out;
        }
        return "";
    }, value);
}

bool glimmer::SettingDef::SetFromString(const std::string &text) {
    switch (type) {
        case SettingType::Bool:
            value = (text == "true" || text == "1");
            return true;
        case SettingType::Int: {
            try {
                value = std::stoi(text);
            } catch (...) {
                return false;
            }
            return true;
        }
        case SettingType::IntRange: {
            try {
                value = static_cast<int>(std::stof(text));
            } catch (...) {
                return false;
            }
            return true;
        }
        case SettingType::Float:
        case SettingType::FloatRange: {
            try {
                value = std::stof(text);
            } catch (...) {
                return false;
            }
            return true;
        }
        case SettingType::List: {
            std::vector<std::string> list;
            std::string_view view(text);
            size_t start = 0;
            while (start <= view.size()) {
                const size_t comma = view.find(',', start);
                const std::string_view token = view.substr(
                    start, comma == std::string_view::npos ? std::string_view::npos : comma - start);
                if (!token.empty()) {
                    list.emplace_back(token);
                }
                if (comma == std::string_view::npos) {
                    break;
                }
                start = comma + 1;
            }
            value = std::move(list);
            return true;
        }
        case SettingType::Enum:
        case SettingType::String:
        default:
            value = text;
            return true;
    }
}

bool glimmer::Config::Load(const toml::value &v) {
    tags_.clear();
    settings_.clear();
    settingsByKey_.clear();
    commandHooks_.clear();

    if (v.contains("tags")) {
        for (const auto &tagValue: toml::find<std::vector<toml::value> >(v, "tags")) {
            TagDef tag;
            tag.tagId = toml::find<std::string>(tagValue, "tag_id");
            tag.labelKey = toml::find_or<std::string>(tagValue, "displayName", tag.tagId);
            tag.enable = toml::find_or<bool>(tagValue, "enable", false);
            tag.visible = toml::find_or<bool>(tagValue, "visible", false);
            tags_.push_back(std::move(tag));
        }
    }

    if (v.contains("settings")) {
        for (const auto &settingValue: toml::find<std::vector<toml::value> >(v, "settings")) {
            SettingDef setting;
            setting.key = toml::find<std::string>(settingValue, "key");
            setting.labelKey = toml::find_or<std::string>(settingValue, "displayName", setting.key);
            setting.groupTag = toml::find_or<std::string>(settingValue, "group_tag", "");
            setting.requiredTags = toml::find_or<std::vector<std::string> >(settingValue, "required_tags", {});
            setting.type = ParseSettingType(toml::find_or<std::string>(settingValue, "type", "string"));
            setting.options = toml::find_or<std::vector<std::string> >(settingValue, "options", {});
            setting.minValue = ParseOptionalFloat(settingValue, "min_value");
            setting.maxValue = ParseOptionalFloat(settingValue, "max_value");
            setting.stepValue = ParseOptionalFloat(settingValue, "step_value");
            setting.defaultValue = ParseValue(settingValue, setting.type, "default_value");
            if (settingValue.contains("value")) {
                setting.value = ParseValue(settingValue, setting.type, "value");
            } else {
                setting.value = setting.defaultValue;
            }
            settings_.push_back(std::move(setting));
        }
    }

    for (SettingDef &setting: settings_) {
        settingsByKey_[setting.key] = &setting;
    }

    commandHooks_ = toml::find_or<std::vector<CommandHookResource> >(v, "command_hooks", {});

    fingerprint_++;
    return true;
}

toml::value glimmer::Config::Save() const {
    toml::value root(toml::table{});

    root["version"] = toml::value(1);

    toml::array tagArray;
    for (const auto &tag: tags_) {
        toml::table table;
        table["displayName"] = toml::value(tag.labelKey);
        table["tag_id"] = toml::value(tag.tagId);
        table["enable"] = toml::value(tag.enable);
        table["visible"] = toml::value(tag.visible);
        tagArray.emplace_back(table);
    }
    root["tags"] = toml::value(tagArray);

    toml::array settingArray;
    for (const auto &setting: settings_) {
        toml::table table;
        table["displayName"] = toml::value(setting.labelKey);
        table["key"] = toml::value(setting.key);
        table["group_tag"] = toml::value(setting.groupTag);
        toml::array requiredArray;
        for (const auto &tag: setting.requiredTags) {
            requiredArray.emplace_back(tag);
        }
        table["required_tags"] = toml::value(requiredArray);
        table["type"] = toml::value(SettingTypeToString(setting.type));
        table["default_value"] = ValueToToml(setting.defaultValue);
        table["value"] = ValueToToml(setting.value);
        if (!setting.options.empty()) {
            toml::array optionArray;
            for (const auto &option: setting.options) {
                optionArray.emplace_back(option);
            }
            table["options"] = toml::value(optionArray);
        }
        if (setting.minValue.has_value()) {
            table["min_value"] = toml::value(*setting.minValue);
        }
        if (setting.maxValue.has_value()) {
            table["max_value"] = toml::value(*setting.maxValue);
        }
        if (setting.stepValue.has_value()) {
            table["step_value"] = toml::value(*setting.stepValue);
        }
        settingArray.emplace_back(table);
    }
    root["settings"] = toml::value(settingArray);

    toml::array hookArray;
    for (const auto &hook: commandHooks_) {
        toml::table table;
        table["hook_id"] = toml::value(hook.hookId);
        table["command"] = toml::value(hook.command);
        table["code"] = toml::value(hook.code);
        table["event_type"] = toml::value(hook.eventType);
        table["key_repeat"] = toml::value(hook.keyRepeat);
        hookArray.emplace_back(table);
    }
    root["command_hooks"] = toml::value(hookArray);

    return root;
}
