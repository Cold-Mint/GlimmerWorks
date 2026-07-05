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
#include <string>
#include <unordered_map>

#include "Command.h"
#include "CommandSender.h"
#include "core/LangsResources.h"
#include "core/utils/TransparentStringHash.h"

namespace glimmer
{
    class DynamicSuggestionsManager;
    class EntityManager;
    class EntityShortCut;

    class CommandManager
    {
        std::unordered_map<std::string, std::unique_ptr<Command>, TransparentStringHash, std::equal_to<>> commandMap_{};
        CommandSender defaultCommandSender_;
        CommandSender mouseCommandSender_;
        WorldContext* worldContext_ = nullptr;
        EntityManager* entityManager_ = nullptr;
        EntityShortCut* entityShortCut_ = nullptr;

    public:
        void RegisterCommand(std::unique_ptr<Command> command);

        Command* GetCommand(const std::string& name) const;

        [[nodiscard]] CommandSender* GetDefaultCommandSender();

        [[nodiscard]] CommandSender* GetMouseCommandSender();

        void BindWorldContext(WorldContext* worldContext);

        void UnbindWorldContext();

        [[nodiscard]] std::string GetHelpText(const LangsResources* langsResources);

        std::vector<std::string> GetSuggestions(const DynamicSuggestionsManager* dynamicSuggestionsManager,
                                                const CommandArgs& commandArgs, int cursorPos) const;

        /**
         * Get Command Structure
         * 获取命令结构
         * @param commandArgs commandArgs 命令参数
         * @return
         */
        std::vector<std::string> GetCommandStructure(const CommandArgs* commandArgs) const;

        /**
         * Extended suggestions
         * 扩展建议
         * @param dynamicSuggestionsManager Dynamic Suggestion Manager 动态建议管理器
         * @param nextNodeTree Fully expand the dynamic suggestions of the nodes. 将节点的动态建议完全展开。
         * @return The expanded list of suggestions 展开后的建议列表
         */
        static std::vector<std::string> ExtendSuggestions(const DynamicSuggestionsManager* dynamicSuggestionsManager,
                                                          const NodeTree<std::string>* nextNodeTree);
    };
}
