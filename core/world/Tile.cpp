//
// Created by Cold-Mint on 2025/11/2.
//

#include "Tile.h"

#include <optional>

#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"
#include "core/log/LogCat.h"


const glimmer::ResourceRef &glimmer::Tile::GetLootTableRef() {
    return lootTable_;
}

float glimmer::Tile::GetMinLightBrightness() const {
    return minLightBrightness_;
}

bool glimmer::Tile::IsLightPeakAtCenter() const {
    return lightPeakAtCenter_;
}

const SDL_Color &glimmer::Tile::GetLightTransmissionColor() const {
    return lightTransmissionColor_;
}

const glimmer::ResourceRef &glimmer::Tile::GetResourceRef() {
    return tileRef_;
}

bool glimmer::Tile::IsCustomLootTable() const {
    return customLootTable_;
}

SDL_Color glimmer::Tile::GetLightColor() const {
    return lightColor_;
}

SDL_Color glimmer::Tile::GetEmissionColor() const {
    return emissionColor_;
}

int glimmer::Tile::GetEmissionRadius() const {
    return emissionRadius_;
}

void glimmer::Tile::SetLightColor(const SDL_Color lightColor) {
    lightColor_ = lightColor;
}

bool glimmer::Tile::SetLayerType(const TileLayerType layerType) {
    bool result = false;
    if (layerType_ != layerType) {
        if (allowCrossLayerPlacement_) {
            layerType_ = layerType;
            result = true;
        }
#if  !defined(NDEBUG)
        else {
            LogCat::e("Attempt to modify the fixed layer.");
            assert(false);
        }
#endif
    }
    return result;
}


bool glimmer::Tile::IsAllowCrossLayerPlacement() const {
    return allowCrossLayerPlacement_;
}

glimmer::TilePhysicsType glimmer::Tile::GetTilePhysicsType() const {
    return physicsType_;
}

bool glimmer::Tile::IsAllowChainMining() const {
    return allowChainMining_;
}

const std::string &glimmer::Tile::GetId() const {
    return id_;
}

SDL_Texture *glimmer::Tile::GetTexture() const {
    return texture_.get();
}

MIX_Audio *glimmer::Tile::GetBreakSFX() const {
    return breakSFX_.get();
}

MIX_Audio *glimmer::Tile::GetPlaceSFX() const {
    return placeSFX_.get();
}

void glimmer::Tile::SetPlayerPlaced(const bool playerPlaced) {
    isPlayerPlaced_ = playerPlaced;
}

bool glimmer::Tile::IsPlayerPlaced() const {
    return isPlayerPlaced_;
}

const std::optional<std::string> &glimmer::Tile::GetDescription() const {
    return description_;
}


const float &glimmer::Tile::GetHardness() const {
    return hardness_;
}

bool glimmer::Tile::IsBreakable() const {
    return breakable;
}

const std::string &glimmer::Tile::GetName() const {
    return name_;
}

glimmer::TileLayerType glimmer::Tile::GetLayerType() const {
    return layerType_;
}

void glimmer::Tile::ReadTileMessage(const TileMessage &tileMessage) {
    tileRef_.ReadResourceRefMessage(tileMessage.resourceref());
    isPlayerPlaced_ = tileMessage.isplayerplaced();
}

void glimmer::Tile::WriteTileMessage(TileMessage &tileMessage) const {
    tileRef_.WriteResourceRefMessage(*tileMessage.mutable_resourceref());
    tileMessage.set_isplayerplaced(isPlayerPlaced_);
}

std::unique_ptr<glimmer::Tile> glimmer::Tile::FromTileResource(const AppContext *appContext,
                                                               const TileResource *tileResource,
                                                               const ResourceRef &resourceRef) {
    auto tile = std::make_unique<Tile>();
    ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    tile->tileRef_ = resourceRef;
    tile->id_ = Resource::GenerateId(*tileResource);
    const StringResource *nameStringResource = resourceLocator->FindString(
        tileResource->name);
    if (nameStringResource == nullptr) {
        tile->name_ = tile->id_;
    } else {
        tile->name_ = nameStringResource->value;
    }
    const StringResource *descriptionStringResource = resourceLocator->FindString(
        tileResource->description);
    if (descriptionStringResource != nullptr) {
        tile->description_ = descriptionStringResource->value;
    }
    tile->customLootTable_ = tileResource->customLootTable;
    tile->lootTable_ = tileResource->lootTable;
    tile->layerType_ = static_cast<TileLayerType>(tileResource->layerType);
    tile->physicsType_ = static_cast<TilePhysicsType>(tileResource->physicsType);
    tile->hardness_ = tileResource->hardness;
    tile->breakable = tileResource->hardness >= 0;
    tile->allowChainMining_ = tileResource->allowChainMining;
    tile->allowCrossLayerPlacement_ = tileResource->allowCrossLayerPlacement;
    const ColorResource *lightTransmissionColorResource =
            resourceLocator->FindColorResource(tileResource->lightTransmissionColor);
    if (lightTransmissionColorResource == nullptr) {
        tile->lightTransmissionColor_ = appContext->GetPreloadColors()->light.defaultLightTransmissionColor;
    } else {
        tile->lightTransmissionColor_ = lightTransmissionColorResource->ToSDLColor();
    }
    const ColorResource *emissionColorResource = resourceLocator->FindColorResource(tileResource->emissionColor);
    if (emissionColorResource == nullptr) {
        tile->emissionColor_ = appContext->GetPreloadColors()->light.defaultEmissionColor;
    } else {
        tile->emissionColor_ = emissionColorResource->ToSDLColor();
    }
    tile->minLightBrightness_ = tileResource->minLightBrightness;
    tile->lightPeakAtCenter_ = tileResource->lightPeakAtCenter;
    tile->emissionRadius_ = tileResource->emissionRadius;
    tile->texture_ = resourceLocator->FindTexture(
        tileResource->texture);
    tile->breakSFX_ = resourceLocator->FindAudio(tileResource->breakSfx);
    tile->placeSFX_ = resourceLocator->FindAudio(tileResource->placeSfx);

    return tile;
}
