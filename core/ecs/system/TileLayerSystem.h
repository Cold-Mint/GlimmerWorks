//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILELAYERSYSTEM_H
#define GLIMMERWORKS_TILELAYERSYSTEM_H
#include "../GameSystem.h"
#include "../component/TileLayerComponent.h"
#include "../component/Transform2DComponent.h"

namespace glimmer
{
    class TileLayerSystem final : public GameSystem
    {
    public:
        TileLayerSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<Transform2DComponent>();
            RequireComponent<TileLayerComponent>();
        }

        void Render(SDL_Renderer* renderer) override;

        uint8_t GetRenderOrder() override;


        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_TILELAYERSYSTEM_H
