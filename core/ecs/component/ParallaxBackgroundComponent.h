//
// Created by coldmint on 2026/5/10.
//

#ifndef GLIMMERWORKS_PARALLAXBACKGROUNDCOMPONENT_H
#define GLIMMERWORKS_PARALLAXBACKGROUNDCOMPONENT_H
#include <memory>

#include "core/mod/ResourceLocator.h"
#include "SDL3/SDL_render.h"


namespace glimmer {
    class ParallaxBackgroundComponent : public GameComponent {
        std::shared_ptr<SDL_Texture> texture_ = nullptr;
        ResourceRef textureResourceRef_;
        bool needsUpdate_ = false;

    public:
        [[nodiscard]] uint32_t GetId() override;

        void SetTextureResourceRef(ResourceRef textureResourceRef);

        ResourceRef &GetTextureResourceRef();

        void ClearTexture();

        SDL_Texture *GetTexture(const ResourceLocator *resourceLocator);
    };
}


#endif //GLIMMERWORKS_PARALLAXBACKGROUNDCOMPONENT_H
