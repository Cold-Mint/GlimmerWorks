//
// Created by coldmint on 2026/4/10.
//

#ifndef GLIMMERWORKS_LIGHT2DSYSTEM_H
#define GLIMMERWORKS_LIGHT2DSYSTEM_H
#include "core/ecs/GameSystem.h"
#include "core/world/Tile.h"
#include "core/world/TraverseAction.h"


namespace glimmer {
    class Light2DSystem : public GameSystem {
        [[nodiscard]] std::vector<Tile *> GetTopVisibleTiles(uint8_t layerFilter, TileVector2D tileVector2d) const;

        TraverseAction StepCallback(const Tile *centerTile, TileVector2D current,
                                    TileVector2D next, float distance);

    public:
        explicit Light2DSystem(WorldContext *worldContext);

        void Update(float delta) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_LIGHT2DSYSTEM_H
