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
#include <memory>
#include <unordered_map>
#include <vector>

#include "CommandHookEntry.h"
#include "Config.h"


namespace glimmer
{
    class CommandHookManager
    {
        std::vector<std::unique_ptr<CommandHookEntry>> sessionCommandHookVector_;
        std::vector<std::unique_ptr<CommandHookEntry>> configCommandHookVector_;
        std::unordered_map<uint32_t, std::vector<CommandHookEntry*>> sessionCommandHookMap_;
        std::unordered_map<uint32_t, std::vector<CommandHookEntry*>> configCommandHookMap_;
        std::vector<CommandHookEntry*> fullVector_;


        [[nodiscard]] bool Exist(CommandHookScope scope,
                                 uint32_t key, const std::string& command) const;

        [[nodiscard]] std::optional<std::string> RegisterImpl(
            std::unordered_map<uint32_t, std::vector<CommandHookEntry*>>& commandHookMap,
            std::vector<std::unique_ptr<CommandHookEntry>>& commandHookVector, CommandHookScope exclude,
            std::unique_ptr<CommandHookEntry> commandHookEntry) const;

        [[nodiscard]] static bool UnregisterImpl(
            std::unordered_map<uint32_t, std::vector<CommandHookEntry*>>& commandHookMap,
            std::vector<std::unique_ptr<CommandHookEntry>>& commandHookVector, CommandHookScope exclude,
            const std::string& commandHookId
        );

    public:
        void LoadHookFromConfig(const std::vector<CommandHookResource>& commandHooks);


        [[nodiscard]] const std::vector<CommandHookEntry*>& GetCommandHookVector(uint32_t key);

        /**
         * CreateCommandHookEntry
         * 创建命令钩子
         * @param scope scope 作用域
         * @param eventType eventType 事件类型
         * @param code code 扫描码
         * @param command command 命令
         * @param keyRepeat keyRepeat 是否在重复输入时执行。
         * @return If the hook does not exist, then return the created hook. If it exists, return an empty value. 如果钩子不存在，那么返回创建后的钩子。存在返回空。
         */
        [[nodiscard]] std::unique_ptr<CommandHookEntry> CreateCommandHookEntry(
            CommandHookScope scope, SDL_EventType eventType, uint16_t code, const std::string& command,
            bool keyRepeat) const;


        /**
         * Contains
         * 是否包含hookId
         * @param hookId 钩子Id
         * @return
         */
        [[nodiscard]] bool Contains(const std::string& hookId) const;

        [[nodiscard]] std::vector<std::string> GetCommandHookIdsWithOutConfig() const;


        [[nodiscard]] std::optional<std::string> Register(std::unique_ptr<CommandHookEntry> commandHookEntry);

        [[nodiscard]] bool Unregister(const std::string& commandHookId);
    };
}
