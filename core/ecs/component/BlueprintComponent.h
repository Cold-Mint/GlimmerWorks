//
// Created by coldmint on 2026/5/23.
//

#pragma once
#include "TileLayerComponent.h"
#include "core/ecs/GameComponent.h"


namespace glimmer {
    class BlueprintComponent : public GameComponent {
        bool canPlace_ = false;
        TileVector2D topLeftVector_;

    public:
        void SetCanPlace(bool canPlace);

        [[nodiscard]] bool CanPlace() const;

        void SetTopLeftVector(const TileVector2D &topLeftVector);

        [[nodiscard]] TileVector2D GetTopLeftVector() const;

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
