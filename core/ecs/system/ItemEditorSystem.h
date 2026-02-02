//
// Created by coldmint on 2026/1/23.
//

#ifndef GLIMMERWORKS_ITEMEDITORSYSTEM_H
#define GLIMMERWORKS_ITEMEDITORSYSTEM_H
#include "core/ecs/GameSystem.h"

namespace glimmer {
    class ItemEditorSystem : public GameSystem {
        [[nodiscard]] bool ShouldActivate() override;

    public:
        ItemEditorSystem(AppContext *appContext, WorldContext *worldContext);

        bool HandleEvent(const SDL_Event &event) override;

        std::string GetName() override;

        uint8_t GetRenderOrder() override;
    };
}

#endif //GLIMMERWORKS_ITEMEDITORSYSTEM_H
