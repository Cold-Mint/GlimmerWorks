//
// Created by Cold-Mint on 2025/12/22.
//

#ifndef GLIMMERWORKS_AUTOPICKSYSTEM_H
#define GLIMMERWORKS_AUTOPICKSYSTEM_H
#include "../GameSystem.h"
#include "../component/ItemContainerComonent.h"

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

#endif //GLIMMERWORKS_AUTOPICKSYSTEM_H
