//
// Created by coldmint on 2026/4/24.
//

#include "LightContribution.h"


void glimmer::LightContribution::SetRayIndex(int rayIndex) {
    rayIndex_ = rayIndex;
}

int glimmer::LightContribution::GetRayIndex() const {
    return rayIndex_;
}

void glimmer::LightContribution::SetLightSource(const LightSource *lightSource) {
    lightSource_ = lightSource;
}

const glimmer::LightSource *glimmer::LightContribution::GetLightSource() const {
    return lightSource_;
}

void glimmer::LightContribution::SetLightColor(std::unique_ptr<SDL_Color> lightColor) {
    lightColor_ = std::move(lightColor);
}

std::unique_ptr<SDL_Color> glimmer::LightContribution::TakeLightColor() {
    return std::move(lightColor_);
}

const SDL_Color *glimmer::LightContribution::GetLightColor() const {
    return lightColor_.get();
}
