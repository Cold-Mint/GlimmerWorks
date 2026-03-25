//
// Created by Cold-Mint on 2026/3/3.
//

#include "SpiritRendererComponent.h"

glimmer::SpiritRendererComponent::SpiritRendererComponent() = default;

void glimmer::SpiritRendererComponent::SetTextureRef(const ResourceRef &resourceRef) {
    resourceRef_ = resourceRef;
}

bool glimmer::SpiritRendererComponent::IsFlipH() const {
    return flipH_;
}

bool glimmer::SpiritRendererComponent::IsFlipV() const {
    return flipV_;
}

void glimmer::SpiritRendererComponent::SetFlipH(const bool flipH) {
    flipH_ = flipH;
}

void glimmer::SpiritRendererComponent::SetFlipV(const bool flipV) {
    flipV_ = flipV;
}

SDL_Texture *glimmer::SpiritRendererComponent::GetTexture(const ResourceLocator *resourceLocator) {
    if (texture_ == nullptr) {
        texture_ = resourceLocator->FindTexture(resourceRef_);
    }
    return texture_.get();
}

const glimmer::ResourceRef &glimmer::SpiritRendererComponent::GetTextureRef() const {
    return resourceRef_;
}

void glimmer::SpiritRendererComponent::SetPosition(const Vector2D position) {
    position_ = position;
}

uint32_t glimmer::SpiritRendererComponent::GetId() {
    return COMPONENT_ID_SPIRIT_RENDERER;
}

const glimmer::Vector2D &glimmer::SpiritRendererComponent::GetPosition() const {
    return position_;
}
