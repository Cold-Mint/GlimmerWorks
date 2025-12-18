//
// Created by coldmint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARSYSTEM_H
#define GLIMMERWORKS_HOTBARSYSTEM_H
#include "../GameSystem.h"
#include "../component/ItemContainerComonent.h"
#include "../component/PlayerControlComponent.h"

namespace glimmer {
    /**
     * HotBar
     * 物品快捷栏
     */
    class HotBarSystem : public GameSystem {
    public:
        HotBarSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
            RequireComponent<PlayerControlComponent>();
            RequireComponent<ItemContainerComponent>();
        }

        void Render(SDL_Renderer *renderer) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_HOTBARSYSTEM_H
