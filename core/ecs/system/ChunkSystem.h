//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNKSYSTEM_H
#define GLIMMERWORKS_CHUNKSYSTEM_H
#include "../GameSystem.h"

namespace glimmer
{
    class ChunkSystem final : public GameSystem
    {
    public:
        ChunkSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
        }

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_CHUNKSYSTEM_H
