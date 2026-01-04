//
// Created by coldmint on 2025/12/25.
//

#ifndef GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H
#define GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H

#include "ItemAbility.h"

namespace glimmer {
    class DigAbility : public ItemAbility {
        /**
         * efficiency
         * 工具效率
         */
        float efficiency_ = 1.0F;

    public:
        explicit DigAbility(
            const VariableConfig &abilityData) : ItemAbility(
            abilityData) {
            auto efficiencyVariable = abilityData.FindVariable("efficiency");
            if (efficiencyVariable != nullptr) {
                efficiency_ = efficiencyVariable->AsFloat();
            }
        }

        ~DigAbility() override = default;

        [[nodiscard]] std::string GetId() const override;

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) override;

        [[nodiscard]] std::unique_ptr<ItemAbility> Clone() const override {
            return std::make_unique<DigAbility>(*this);
        }
    };
}


#endif //GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H
