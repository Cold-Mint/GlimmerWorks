//
// Created by coldmint on 2026/4/4.
//

#ifndef GLIMMERWORKS_DEBUGMULTIMAPSYSTEM_H
#define GLIMMERWORKS_DEBUGMULTIMAPSYSTEM_H
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/TileLayerComponent.h"


namespace glimmer {
    class DebugMultiMapSystem : public GameSystem {
        [[nodiscard]] bool ShouldActivate() override;

        SDL_Color GetTileDebugColor(TileVector2D tile);

    public:
        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;

        explicit DebugMultiMapSystem(WorldContext *worldContext);

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_DEBUGMULTIMAPSYSTEM_H
