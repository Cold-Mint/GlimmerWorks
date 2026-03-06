//
// Created by coldmint on 2026/3/3.
//

#ifndef GLIMMERWORKS_SPIRITRENDERERCOMPONENT_H
#define GLIMMERWORKS_SPIRITRENDERERCOMPONENT_H

#include "core/ecs/GameComponent.h"
#include "core/math/Vector2D.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "SDL3/SDL_render.h"

namespace glimmer {
    class SpiritRendererComponent : public GameComponent {
        ResourceRef resourceRef_;
        Vector2D position_;
        std::shared_ptr<SDL_Texture> texture_ = nullptr;

    public:
        SpiritRendererComponent();

        void SetTextureRef(const ResourceRef &resourceRef);

        [[nodiscard]] SDL_Texture *GetTexture(const ResourceLocator *resourceLocator);

        [[nodiscard]] const ResourceRef &GetTextureRef() const;

        void SetPosition(Vector2D position);

        [[nodiscard]] uint32_t GetId() override;

        [[nodiscard]] const Vector2D &GetPosition() const;
    };
}

#endif //GLIMMERWORKS_SPIRITRENDERERCOMPONENT_H
