//
// Created by coldmint on 2025/12/25.
//

#ifndef GLIMMERWORKS_ITEMMODEDITORSYSTEM_H
#define GLIMMERWORKS_ITEMMODEDITORSYSTEM_H

#include "../GameSystem.h"
#include "../../inventory/ComposableItem.h"

namespace glimmer {
    class ItemModEditorSystem : public GameSystem {
        bool isVisible_ = false;
        ComposableItem *editingItem_ = nullptr;

        [[nodiscard]] bool ShouldActivate() override;

    public:
        ItemModEditorSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
        }

        void Render(SDL_Renderer *renderer) override;

        bool HandleEvent(const SDL_Event &event) override;

        uint8_t GetRenderOrder() override;

        std::string GetName() override;

    private:
        void ToggleEditor();
    };
}

#endif //GLIMMERWORKS_ITEMMODEDITORSYSTEM_H
