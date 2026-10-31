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
#include "core/config/Config.h"
#include "core/utils/LangsResources.h"
#include "core/console/Command.h"
#include "core/vfs/VirtualFileSystem.h"
#include "toml11/types.hpp"
#include "fmt/color.h"

namespace glimmer {
    struct LangsResources;

    class ConfigCommand final : public Command {
        Config *config_ = nullptr;
        VirtualFileSystem *virtualFileSystem_ = nullptr;
        uint64_t resourcePackPath_ = StringUtils::StringToUint64("resource_pack_path");
        uint64_t dataPackPath_ = StringUtils::StringToUint64("data_pack_path");

        void InitSuggestions(NodeTree<std::string> *suggestionsTree) override;

        void UpdateSetSuggestions(const CommandArgs *commandArgs);

        static bool ExecuteCommit(const VirtualFileSystem *virtualFileSystem, Config *config,
                                  const LangsResources *langsResources,
                                  const std::function<void(const std::string &text)> &onMessageRef);

        template<typename F>
        static bool ExecuteGet(int size, const CommandArgs *commandArgs, const LangsResources *langsResources,
                               const ConfigCommand *command, F &&onMessageRef) {
            if (size < 3) {
                onMessageRef(fmt::format(
                    fmt::runtime(langsResources->insufficientParameterLength),
                    3, size));
                return false;
            }
            std::optional<std::string> stringOptional = command->GetValue(
                StringUtils::StringToUint64(commandArgs->AsString(2)));
            if (!stringOptional.has_value()) {
                return false;
            }
            onMessageRef(stringOptional.value());
            return true;
        }

        template<typename F>
        bool ExecuteSet(int size, const CommandArgs *commandArgs, const LangsResources *langsResources,
                        const ConfigCommand *command, F &&onMessageRef) {
            if (size < 4) {
                onMessageRef(fmt::format(
                    fmt::runtime(langsResources->insufficientParameterLength),
                    4, size));
                return false;
            }

            std::string parameterName = commandArgs->AsString(2);
            const uint64_t settingId = StringUtils::StringToUint64(parameterName);
            std::string value = commandArgs->AsString(3);
            const auto settingDef = GetSettingDef(settingId);
            if (settingDef == nullptr) {
                return false;
            }
            if (const SettingType settingType = settingDef->type;
                settingType == SettingType::Bool && value == TOGGLE_KEY_WORD) {
                const std::optional<std::string> oldValue = command->GetValue(settingId);
                value = oldValue == "true" ? "false" : "true";
            }
            if (command->SetValue(settingId, value)) {
                onMessageRef(fmt::format(fmt::runtime(langsResources->configurationUpdate),
                                         parameterName, value));
            }
            return true;
        }

    public:
        explicit ConfigCommand(AppContext *appContext);

        ~ConfigCommand() override = default;

        [[nodiscard]] const std::string &GetName() const override;

        void PutCommandStructure(const CommandArgs *commandArgs, std::vector<std::string> *strings) override;

        [[nodiscard]] NodeTree<std::string> *GetSuggestionsTree(const CommandArgs *commandArgs) override;

        bool Execute(const CommandSender *commandSender, const CommandArgs *commandArgs,
                     const std::function<void(const std::string &text)> *onMessage) override;

        [[nodiscard]] const SettingDef *GetSettingDef(uint64_t settingId) const;

        [[nodiscard]] std::optional<std::string> GetValue(uint64_t settingId) const;

        [[nodiscard]] bool SetValue(uint64_t settingId, const std::string &value) const;
    };
}
