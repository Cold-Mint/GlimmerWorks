//
// Created by coldmint on 2026/4/15.
//

#include "LightMask.h"


glimmer::LightMask::LightMask(const TileVector2D &position, const TileLayerType &tileLayer,
                              const SDL_Color &lightMaskColor) {
    position_ = position;
    tileLayer_ = tileLayer;
    lightMaskColor_ = lightMaskColor;
}

const glimmer::TileLayerType &glimmer::LightMask::GetTileLayer() const {
    return tileLayer_;
}

const SDL_Color &glimmer::LightMask::GetLightMaskColor() const {
    return lightMaskColor_;
}

const TileVector2D &glimmer::LightMask::GetPosition() const {
    return position_;
}
