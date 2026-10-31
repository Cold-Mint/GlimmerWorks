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
#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core/utils/VersionAble.h"
#include "toml11/types.hpp"


namespace glimmer {
    /**
     * SettingType
     * 设置项类型。决定设置页使用哪种控件、以及 toml 值的解析方式。
     */
    enum class SettingType {
        String,
        Bool,
        Int,
        Float,
        Enum,
        IntRange,
        FloatRange,
        List,
        Path
    };

    /**
     * SettingValue
     * 设置项的值（强类型 variant）。
     */
    using SettingValue = std::variant<bool, int, float, std::string, std::vector<std::string> >;

    /**
     * SettingDef
     * 一条自描述的设置项，直接由 config.toml 的 [[settings]] 解析而来。
     */
    struct SettingDef {
        std::string key;
        std::string labelKey;
        std::string groupTag;
        std::vector<std::string> requiredTags;
        SettingType type = SettingType::String;
        SettingValue value;
        SettingValue defaultValue;
        std::vector<std::string> options;
        std::optional<float> minValue;
        std::optional<float> maxValue;
        std::optional<float> stepValue;

        [[nodiscard]] std::optional<std::string> GetAsString() const;

        bool SetFromString(const std::string &text);
    };

    /**
     * TagDef
     * 一个设置分组（标签），直接由 config.toml 的 [[tags]] 解析而来。
     */
    struct TagDef {
        std::string tagId;
        std::string labelKey;
        bool enable = false;
        bool visible = false;
    };

    /**
     * CommandHookResource
     * 命令钩子资源。在 config.toml 中以 [[command_hooks]] 原始透传，不参与设置页。
     */
    struct CommandHookResource {
        std::string hookId;
        std::string command;
        std::string code;
        std::string eventType;
        bool keyRepeat;
    };

    /**
     * Config
     * 自描述的配置注册表：由 config.toml 的 [[tags]] + [[settings]] + [[command_hooks]]
     * 解析而来，同时驱动运行时访问、设置页生成与序列化。
     */
    class Config : public VersionAble {
        std::vector<TagDef> tags_;
        std::vector<SettingDef> settings_;
        std::unordered_map<uint64_t, TagDef *> tagDefMap_;
        std::unordered_map<uint64_t, SettingDef *> settingsByKey_;
        std::vector<CommandHookResource> commandHooks_;

    public:
        [[nodiscard]] const std::vector<TagDef> &Tags() const { return tags_; }

        [[nodiscard]] std::vector<TagDef> &Tags() { return tags_; }

        TagDef *FindMutableTag(uint64_t tagId) const;

        void SetTagEnabled(uint64_t tagId, bool enable);

        [[nodiscard]] const std::vector<SettingDef> &Settings() const { return settings_; }

        [[nodiscard]] std::vector<SettingDef> &Settings() { return settings_; }

        [[nodiscard]] const std::vector<CommandHookResource> &CommandHooks() const { return commandHooks_; }

        [[nodiscard]] SettingDef *FindMutableSettingDef(uint64_t settingId);

        [[nodiscard]] const SettingDef *FindSettingDef(uint64_t settingId) const;

        /**
         * Read a setting by flattened key. Returns T{} if the key is missing or
         * the value type does not match.
         * 按扁平 key 读取设置值。key 缺失或类型不匹配时返回 T{}。
         */
        template<typename T>
        [[nodiscard]] T Get(const uint64_t settingId) const {
            const SettingDef *setting = FindSettingDef(settingId);
            if (setting == nullptr) {
                return T{};
            }
            if (const T *value = std::get_if<T>(&setting->value)) {
                return *value;
            }
            return T{};
        }

        /**
         * Write a setting by flattened key and mark the config dirty.
         * 按扁平 key 写入设置值，并标记配置已变更。
         */
        template<typename T>
        void Set(const uint64_t settingId, T value) {
            SettingDef *setting = FindMutableSettingDef(settingId);
            if (setting == nullptr) {
                return;
            }
            setting->value = std::move(value);
            AddVersion();
        }

        /**
         * Read a setting value as its string representation.
         * 以字符串形式读取设置值。
         */
        [[nodiscard]] std::optional<std::string> GetAsString(uint64_t settingId) const;

        /**
         * Write a setting value from its string representation.
         * 以字符串形式写入设置值。
         */
        bool SetFromString(uint64_t settingId, const std::string &text);

        /**
         * Read a list setting as a list of uint64 (converting each element via
         * StringUtils::StringToUint64). Used for enabled pack ids.
         * 将 list 类型的设置读取为 uint64 列表（逐个元素用 StringUtils::StringToUint64
         * 转换）。用于启用的包 ID 列表。
         */
        [[nodiscard]] std::vector<uint64_t> GetUint64List(uint64_t settingId) const;


        /**
         * Parse the new config.toml structure.
         * 解析新的 config.toml 结构。
         */
        bool Load(const toml::value &v);

        /**
         * Serialize the current state back into the config.toml structure.
         * 将当前状态序列化回 config.toml 结构。
         */
        [[nodiscard]] toml::value Save() const;
    };
} // namespace glimmer
