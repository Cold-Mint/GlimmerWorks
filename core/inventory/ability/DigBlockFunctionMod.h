//
// Created by coldmint on 2025/12/25.
//

#ifndef GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H
#define GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H

#include "ItemAbility.h"

namespace glimmer {
    class DigBlockFunctionMod : public ItemAbility {

    public:
        DigBlockFunctionMod() {
            name_ = "Dig Block";
            description_ = "Digs blocks in front of the player.";
        }
        [[nodiscard]] std::string GetId() const override;

        void OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) override;
    };
}


#endif //GLIMMERWORKS_DIGBLOCKFUNCTIONMOD_H