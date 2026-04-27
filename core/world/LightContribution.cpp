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

void glimmer::LightContribution::SetLightColor(std::unique_ptr<Color> lightColor) {
    lightColor_ = std::move(lightColor);
}

std::unique_ptr<glimmer::Color> glimmer::LightContribution::TakeLightColor() {
    return std::move(lightColor_);
}

const glimmer::Color *glimmer::LightContribution::GetLightColor() const {
    return lightColor_.get();
}
