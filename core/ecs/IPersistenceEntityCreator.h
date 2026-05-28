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
#include "GameEntity.h"
#include "core/mod/ResourceRef.h"
#include "core/world/WorldContext.h"
#include "src/saves/entity_item.pb.h"

namespace glimmer {
    /**
     * IPersistenceEntityCreator
     * 持久化实体创建器
     */
    class IPersistenceEntityCreator {
    protected:
        WorldContext *worldContext_ = nullptr;

        /**
         * RecoveryAllComponent
         * 恢复所有的组件
         * @param id id 实体id
         * @param entityItemMessage entityItemMessage 实体项目信息
         */
        void RecoveryAllComponent(GameEntity::ID id,
                                  const EntityItemMessage &entityItemMessage) const;

    public:
        explicit IPersistenceEntityCreator(WorldContext *worldContext);

        virtual ~IPersistenceEntityCreator() = default;

        /**
         * LoadTemplateComponents
         * 加载模板组件
         *
         * This method typically adds the corresponding component information based on the resources defined in the data package.
         * 这个方法通常从数据包定义的资源来添加对应的组件信息。
         */
        virtual void LoadTemplateComponents(GameEntity::ID id, const ResourceRef &resourceRef) = 0;

        /**
         * Merge the data of the entity projects
         * 合并实体项目数据
         *
         * This method corresponds to loading dynamic data from the archive and overwriting the static data definitions within the template.
         * 这个方法对应，从存档内加载动态数据，并覆盖模板内的静态数据定义。
         *
         *For example: The current health value of the health component (which needs to be saved in the save file during runtime) is called dynamic data.
         * 比如：健康组件的当前健康值（运行时，需要保存到存档内的）这些数据叫做动态数据。
         *
         * @param id
         * @param entityItemMessage
         */
        virtual void MergeEntityItemMessage(GameEntity::ID id, const EntityItemMessage &entityItemMessage) = 0;
    };
}
