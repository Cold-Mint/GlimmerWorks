//
// Created by coldmint on 2026/4/2.
//

#ifndef GLIMMERWORKS_DRAGGABLESYSTEM_H
#define GLIMMERWORKS_DRAGGABLESYSTEM_H
#include "core/ecs/GameEntity.h"
#include "core/ecs/GameSystem.h"
#include "core/ecs/component/CameraComponent.h"
#include "core/inventory/Item.h"
#include "core/math/Vector2D.h"


namespace glimmer {
    class DraggableSystem : public GameSystem {
        std::unique_ptr<Item> item_;

        SDL_FRect DraggableBorder(GameEntity::ID entityId, WorldVector2D cameraPosition,
                                  const CameraComponent *cameraComponent) const;

        [[nodiscard]] bool ShouldActivate() override;

        void OnActivationChanged(bool activeStatus) override;


        bool DropItem();

    public:
        explicit DraggableSystem(WorldContext *worldContext);

        uint8_t GetRenderOrder() override;

        void Render(SDL_Renderer *renderer) override;

        bool HandleEvent(const SDL_Event &event) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_DRAGGABLESYSTEM_H
