//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNKSYSTEM_H
#define GLIMMERWORKS_CHUNKSYSTEM_H
#include <stack>

#include "../GameSystem.h"
#include "core/math/Vector2D.h"
#include "core/world/ChunkTask.h"

namespace glimmer {
    class ChunkSystem final : public GameSystem {
        WorldVector2D cameraPosition_;
        float accumTime_ = 0.0F;
        bool firstTime_ = true;
        std::deque<ChunkTask> chunkTasks_;
        std::unordered_set<uint64_t> taskIdSet_;

    public:
        explicit ChunkSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_CHUNKSYSTEM_H
