//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#define GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#include "../GameSystem.h"
#include "../component/WorldPositionComponent.h"

namespace glimmer
{
    class WorldPositionSystem final : public GameSystem
    {
    public:
        explicit WorldPositionSystem(WorldContext* worldContext)
            : GameSystem(worldContext)
        {
            RequireComponent<WorldPositionComponent>();
        }

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
