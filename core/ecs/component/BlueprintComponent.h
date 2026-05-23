//
// Created by coldmint on 2026/5/23.
//

#pragma once
#include "core/ecs/GameComponent.h"


namespace glimmer {
    class BlueprintComponent : public GameComponent {
        bool canPlace_ = false;
        

    public:
        void SetCanPlace(bool canPlace);

        [[nodiscard]] bool CanPlace() const;

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
