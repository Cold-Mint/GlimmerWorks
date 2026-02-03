//
// Created by coldmint on 2025/12/25.
//

#ifndef GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H
#define GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H

#include "ItemAbility.h"
#include "core/ecs/GameEntity.h"

namespace glimmer {
    class DigAbility : public ItemAbility {
        /**
         * efficiency
         * 工具效率
         */
        float efficiency_ = 1.0F;

        /**
         * dig Range(Unit: Number of Tile Squares)
         * 挖掘距离（单位：瓦片格数）
         */
        int digRange_ = 5;

    public:
        explicit DigAbility(
            const VariableConfig &abilityData);

        ~DigAbility() override = default;

        [[nodiscard]] std::string GetId() const override;

        void OnUse(WorldContext *worldContext, GameEntity::ID user) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override;
    };
}


#endif //GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H
