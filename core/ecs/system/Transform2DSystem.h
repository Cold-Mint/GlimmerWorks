//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#define GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
#include "../GameSystem.h"
#include "../component/Transform2DComponent.h"

namespace glimmer
{
    class Transform2DSystem final : public GameSystem
    {
    public:
        explicit Transform2DSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<Transform2DComponent>();
        }

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_WORLDPOSITIONSYSTEM_H
