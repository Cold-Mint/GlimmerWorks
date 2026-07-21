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
#include <functional>
#include <string>

#include "CommandArgs.h"
#include "core/utils/NodeTree.h"

namespace glimmer
{
    class CommandSender;
    class WorldContext;
    class AppContext;

    class Command
    {
        AppContext* appContext_ = nullptr;
        WorldContext* worldContext_ = nullptr;
        /**
         * A tree applicable to command suggestions
         * 适用于命令建议的树
         */
        NodeTree<std::string> suggestionsTree_ = NodeTree<std::string>();

        /**
        * Initialize the suggestion tree
        * 初始化建议树
        * @param suggestionsTree
        */
        virtual void InitSuggestions(NodeTree<std::string>* suggestionsTree);

    protected:
        [[nodiscard]] NodeTree<std::string>& GetPrivateSuggestionsTree();

        [[nodiscard]] WorldContext* GetWorldContext() const;

        [[nodiscard]] AppContext* GetAppContext() const;

    public:
        virtual ~Command() = default;


        explicit Command(AppContext* appContext);

        [[nodiscard]] virtual const std::string& GetName() const = 0;

        /**
         * Is the world context required to execute this command
         * 是否需要世界上下文，才能执行该命令
         * @return 是否需要世界上下文
         */
        [[nodiscard]] virtual bool RequiresWorldContext() const;

        /**
        * RequiresCheatEnabled
        * 是否需要开启作弊才能使用此命令
        * @return
        */
        [[nodiscard]] virtual bool RequiresCheatEnabled() const;

        /**
         * Bind the world context
         * 绑定世界上下文
         * @param worldContext 世界上下文
         */
        void BindWorldContext(WorldContext* worldContext);

        /**
         * UnBind the world context
         * 解绑世界上下文
         */
        void UnBindWorldContext();


        /**
         * Add command structure
         * 添加命令结构
         * @param commandArgs commandArgs 命令参数
         * @param strings Command structure array
         */
        virtual void PutCommandStructure(const CommandArgs* commandArgs, std::vector<std::string>* strings);

        /**
         * Initialize
         * 初始化
         */
        void Initialize()
        {
            InitSuggestions(&suggestionsTree_);
        }

        [[nodiscard]] virtual NodeTree<std::string>* GetSuggestionsTree(const CommandArgs* commandArgs);

        virtual bool Execute(const CommandSender* commandSender, const CommandArgs* commandArgs,
                             const std::function<void(const std::string& text)>* onMessage) = 0;
    };
}
