//
// Created by coldmint on 2026/4/2.
//

#ifndef GLIMMERWORKS_DRAGGABLECOMPONENT_H
#define GLIMMERWORKS_DRAGGABLECOMPONENT_H

#include "core/ecs/GameComponent.h"
#include "core/math/Vector2D.h"


namespace glimmer {
    class DraggableComponent : public GameComponent {
        CameraVector2D size_;

    public:
        void SetSize(CameraVector2D size);

        [[nodiscard]] CameraVector2D GetSize() const;

        [[nodiscard]] uint32_t GetId() override;
    };
}


#endif //GLIMMERWORKS_DRAGGABLECOMPONENT_H
