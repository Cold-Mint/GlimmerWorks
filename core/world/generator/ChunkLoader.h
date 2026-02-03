//
// Created by coldmint on 2026/2/3.
//

#ifndef GLIMMERWORKS_WORLDGENERATOR_H
#define GLIMMERWORKS_WORLDGENERATOR_H
#include "core/math/Vector2DI.h"
#include "core/saves/Saves.h"

namespace glimmer {
    class ChunkLoader {
        Saves *saves_;
        WorldContext *worldContext_;

        std::function<bool(std::unique_ptr<GameEntity> entity)> registerEntity_;

        /**
        * Recovery Component
        * 恢复组件
        * @param id id id
        * @param componentMessage componentMessage 组件消息
        * @return
        */
        [[nodiscard]] GameComponent *RecoveryComponent(GameEntity::ID id,
                                                       const ComponentMessage &componentMessage) const;

        /**
         * Load entity from saves
         * 从存档加载实体
         * @param position
         */
        void LoadEntityFromSaves(TileVector2D position) const;

    public:
        ChunkLoader(WorldContext *worldContext, Saves *saves,
                    const std::function<bool(std::unique_ptr<GameEntity> entity)> &registerEntity);

        /**
         * Load block from saves
         * 从存档加载区块
         * @param position position 位置
         */
        [[nodiscard]] std::unique_ptr<Chunk> LoadChunkFromSaves(TileVector2D position) const;

        /**
        * Recovery Entity
        * 恢复实体
        * @param entityItemMessage
        * @return
        */
        [[nodiscard]] GameEntity::ID RecoveryEntity(const EntityItemMessage &entityItemMessage) const;
    };
}

#endif //GLIMMERWORKS_WORLDGENERATOR_H
