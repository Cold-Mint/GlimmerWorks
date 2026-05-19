//
// Created by Cold-Mint on 2025/12/22.
//

#pragma once
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/ItemContainerComonent.h"

namespace glimmer {
    class AutoPickSystem : public GameSystem {
        std::shared_ptr<MIX_Audio> pickItemSFX_;
        WorldVector2D lastPosition{};
        float remainingTime_ = MERGE_DURATION;

        std::unordered_map<std::string, size_t> frameItemCounts_ = {};

    public:
        explicit AutoPickSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}
