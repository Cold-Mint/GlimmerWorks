//
// Created by coldmint on 2026/3/28.
//

#pragma once
#include "core/world/WorldContext.h"

/**
 * IEntityCreator
 * 实体创建器
 */
namespace glimmer {
    class IEntityCreator {
    protected:
        WorldContext *worldContext_ = nullptr;

    public:
        explicit IEntityCreator(WorldContext *worldContext);

        virtual ~IEntityCreator() = default;

        /**
         * LoadTemplateComponents
         * 加载模板组件
         *
         */
        virtual void LoadTemplateComponents(GameEntity::ID id) = 0;
    };
}
