//
// Created by coldmint on 2026/5/10.
//

#include "ParallaxBackgroundComponent.h"

#include <utility>

uint32_t glimmer::ParallaxBackgroundComponent::GetId() {
    return COMPONENT_ID_PARALLAX_BACKGROUND;
}

void glimmer::ParallaxBackgroundComponent::SetTextureResourceRef(ResourceRef textureResourceRef) {
    textureResourceRef_ = std::move(textureResourceRef);
    needsUpdate_ = true;
}

glimmer::ResourceRef &glimmer::ParallaxBackgroundComponent::GetTextureResourceRef() {
    return textureResourceRef_;
}

void glimmer::ParallaxBackgroundComponent::ClearTexture() {
    texture_ = nullptr;
}

SDL_Texture *glimmer::ParallaxBackgroundComponent::GetTexture(const ResourceLocator *resourceLocator) {
    if (needsUpdate_) {
        texture_ = resourceLocator->FindTexture(textureResourceRef_);
        needsUpdate_ = false;
    }
    if (texture_ == nullptr) {
        return nullptr;
    }
    return texture_.get();
}
