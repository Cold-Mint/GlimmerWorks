//
// Created by coldmint on 2026/3/23.
//

#ifndef GLIMMERWORKS_IPERSISTENCEENTITYCREATOR_H
#define GLIMMERWORKS_IPERSISTENCEENTITYCREATOR_H
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

#endif //GLIMMERWORKS_IENTITYCREATOR_H
