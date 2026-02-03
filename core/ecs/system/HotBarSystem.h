//
// Created by Cold-Mint on 2025/12/18.
//

#ifndef GLIMMERWORKS_HOTBARSYSTEM_H
#define GLIMMERWORKS_HOTBARSYSTEM_H
#include "../GameSystem.h"
#include "../component/ItemContainerComonent.h"

namespace glimmer {
    /**
     * HotBar
     * 物品快捷栏
     */
    class HotBarSystem : public GameSystem {
    public:
        explicit HotBarSystem(WorldContext *worldContext);

        void Update(float delta) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;
    };
}

#endif //GLIMMERWORKS_HOTBARSYSTEM_H
