//
// Created by Cold-Mint on 2025/11/2.
//

#include "Tile.h"

#include <optional>

#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"


const glimmer::ResourceRef &glimmer::Tile::GetLootTableRef() {
    return lootTable_;
}

const glimmer::ResourceRef &glimmer::Tile::GetResourceRef() {
    return tileRef_;
}

bool glimmer::Tile::IsCustomLootTable() const {
    return customLootTable_;
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

MIX_Audio *glimmer::Tile::GetBlockPlaceSFX() const {
    return blockPlaceSFX_.get();
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
    return layerType;
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
    tile->tileRef_ = resourceRef;
    tile->id_ = Resource::GenerateId(*tileResource);
    const StringResource *nameStringResource = appContext->GetResourceLocator()->FindString(
        tileResource->name);
    if (nameStringResource == nullptr) {
        tile->name_ = tile->id_;
    } else {
        tile->name_ = nameStringResource->value;
    }
    const StringResource *descriptionStringResource = appContext->GetResourceLocator()->FindString(
        tileResource->description);
    if (descriptionStringResource != nullptr) {
        tile->description_ = descriptionStringResource->value;
    }
    tile->customLootTable_ = tileResource->customLootTable;
    tile->lootTable_ = tileResource->lootTable;
    tile->layerType = static_cast<TileLayerType>(tileResource->layerType);
    tile->physicsType_ = static_cast<TilePhysicsType>(tileResource->physicsType);
    tile->hardness_ = tileResource->hardness;
    tile->breakable = tileResource->hardness >= 0;
    tile->allowChainMining_ = tileResource->allowChainMining;
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator != nullptr) {
        tile->texture_ = resourceLocator->FindTexture(
            tileResource->texture);
        tile->breakSFX_ = resourceLocator->FindAudio(tileResource->breakSFX);
        tile->blockPlaceSFX_ = resourceLocator->FindAudio(tileResource->blockPlaceSFX);
    }

    return tile;
}
