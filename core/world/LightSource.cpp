//
// Created by coldmint on 2026/4/15.
//

#include "LightSource.h"

glimmer::LightSource::LightSource(const TileVector2D &center, const int maxRadius, const SDL_Color &emissionColor,
                                  const TileLayerType &tileLayer) {
    this->center_ = center;
    this->maxRadius_ = maxRadius;
    this->emissionColor_ = emissionColor;
    this->tileLayer_ = tileLayer;
}

int glimmer::LightSource::GetMaxRadius() const {
    return maxRadius_;
}

const glimmer::TileLayerType & glimmer::LightSource::GetTileLayerType() const {
    return tileLayer_;
}

const TileVector2D & glimmer::LightSource::GetCenter() const {
    return center_;
}

const SDL_Color & glimmer::LightSource::GetEmissionColor() const {
    return emissionColor_;
}


