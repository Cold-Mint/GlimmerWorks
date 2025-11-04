//
// Created by Cold-Mint on 2025/11/4.
//

#ifndef GLIMMERWORKS_CHUNKSYSTEM_H
#define GLIMMERWORKS_CHUNKSYSTEM_H
#include "../GameSystem.h"
#include "../component/CameraComponent.h"
#include "../component/PlayerControlComponent.h"
#include "../component/TileLayerComponent.h"
#include "../component/WorldPositionComponent.h"

namespace glimmer
{
    class ChunkSystem final : public GameSystem
    {
        float preloadMargin_ = 1.0F;


    public:
        ChunkSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<WorldPositionComponent>();
            RequireComponent<TileLayerComponent>();
            RequireComponent<CameraComponent>();
            RequireComponent<PlayerControlComponent>();
        }

        void Update(float delta) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_CHUNKSYSTEM_H
