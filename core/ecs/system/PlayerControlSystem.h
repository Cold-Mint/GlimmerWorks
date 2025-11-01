//
// Created by Cold-Mint on 2025/10/29.
//

#ifndef GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
#define GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
#include "../GameSystem.h"
#include "../component/PlayerControlComponent.h"
#include <SDL3/SDL_events.h>

#include "../component/WorldPositionComponent.h"

namespace glimmer
{
    /**
     * The player control system processes the player's input control and realizes the WASD mobile camera function
     * 玩家控制系统，处理玩家的输入控制，实现WASD移动相机功能
     */
    class PlayerControlSystem final : public GameSystem
    {
    public:
        explicit PlayerControlSystem(AppContext* appContext, WorldContext* worldContext)
            : GameSystem(appContext, worldContext)
        {
            RequireComponent<PlayerControlComponent>();
            RequireComponent<WorldPositionComponent>();
        }

        void Update(float delta) override;

        std::string GetName() override;

        bool HandleEvent(const SDL_Event& event) override;
    };
}

#endif //GLIMMERWORKS_PLAYERCONTROLSYSTEM_H
