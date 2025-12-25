//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARSYSTEM_H
#define GLIMMERWORKS_HOTBARSYSTEM_H
#include "../GameSystem.h"
#include "../component/HotBarComonent.h"
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
            RequireComponent<HotBarComponent>();
        }

        void Update(float delta) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_HOTBARSYSTEM_H
