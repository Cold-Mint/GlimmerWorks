//
// Created by Cold-Mint on 2025/12/29.
//

#ifndef GLIMMERWORKS_DIGGINGSYSTEM_H
#define GLIMMERWORKS_DIGGINGSYSTEM_H
#include <memory>
#include <vector>

#include "../GameSystem.h"
#include "core/ecs/component/TileLayerComponent.h"


namespace glimmer {
    class DiggingComponent;

    class DiggingSystem : public GameSystem {
        bool cacheTexture = false;
        std::vector<std::shared_ptr<SDL_Texture> > textureList = {};

        void BreakTile(TileVector2D tilePosition, const AppContext *appContext, const DiggingComponent *diggingComponent,
                       const TileLayerComponent *tileLayerComponent) const;

    public:
        explicit DiggingSystem(WorldContext *worldContext);

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_DIGGINGSYSTEM_H
