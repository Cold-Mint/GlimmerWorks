//
// Created by coldmint on 2026/4/15.
//

#include "LightSource.h"


glimmer::LightSource::LightSource(const TileVector2D &center, int maxRadius, const Color &emissionColor) {
    this->center_ = center;
    this->maxRadius_ = maxRadius;
    this->emissionColor_ = emissionColor;
}

int glimmer::LightSource::GetMaxRadius() const {
    return maxRadius_;
}

const TileVector2D &glimmer::LightSource::GetCenter() const {
    return center_;
}

const glimmer::Color &glimmer::LightSource::GetEmissionColor() const {
    return emissionColor_;
}
