//
// Created by Cold-Mint on 2025/10/28.
//

#ifndef GLIMMERWORKS_GAMESTARTSYSTEM_H
#define GLIMMERWORKS_GAMESTARTSYSTEM_H
#include "../GameSystem.h"

namespace glimmer
{
    class GameStartSystem : public GameSystem
    {
        bool shouldStart = true;

    public:
        explicit GameStartSystem(AppContext* appContext, WorldContext* worldContext);

        void Update(float delta) override;

        std::string GetName() override;

    private:
        [[nodiscard]] bool ShouldActivate() override;
    };
}

#endif //GLIMMERWORKS_GAMESTARTSYSTEM_H
