//
// Created by Cold-Mint on 2026/5/10.
//

#pragma once
#include <memory>
#include "core/ecs/GameComponent.h"
#include "core/mod/ResourceLocator.h"
#include "SDL3/SDL_render.h"


namespace glimmer {
    class ParallaxBackgroundComponent : public GameComponent {
        std::shared_ptr<SDL_Texture> texture_ = nullptr;
        ResourceRef textureResourceRef_;
        bool needsUpdate_ = false;

    public:
        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;

        void SetTextureResourceRef(ResourceRef textureResourceRef);

        ResourceRef &GetTextureResourceRef();

        void ClearTexture();

        SDL_Texture *GetTexture(const ResourceLocator *resourceLocator);
    };
}
