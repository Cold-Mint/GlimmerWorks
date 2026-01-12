//
// Created by coldmint on 2025/12/20.
//

#ifndef GLIMMERWORKS_DROPPEDITEMSYSTEM_H
#define GLIMMERWORKS_DROPPEDITEMSYSTEM_H
#include "../GameSystem.h"
#include "../component/DroppedItemComponent.h"
#include "../component/Transform2DComponent.h"

namespace glimmer {
    class DroppedItemSystem : public GameSystem {
    public:
        DroppedItemSystem(AppContext *appContext, WorldContext *worldContext)
            : GameSystem(appContext, worldContext) {
            RequireComponent<Transform2DComponent>();
            RequireComponent<DroppedItemComponent>();
        }


        uint8_t GetRenderOrder() override;

        void Update(float delta) override;

        void Render(SDL_Renderer *renderer) override;

        std::string GetName() override;
    };
}


#endif //GLIMMERWORKS_DROPPEDITEMSYSTEM_H
