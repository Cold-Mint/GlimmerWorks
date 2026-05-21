//
// Created by Cold-Mint on 2025/11/2.
//

#include "Tile.h"

#include <optional>

#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"


const glimmer::ResourceRef *glimmer::Tile::GetSideLightMaskResource() const {
    return &sideLightMask_;
}

const glimmer::ResourceRef * glimmer::Tile::GetBackLightMaskResource() const {
    return &backLightMask_;
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

bool glimmer::Tile::IsAllowDirAdjustAnchor() const {
    return allowDirAdjustAnchor_;
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


const std::optional<std::string> &glimmer::Tile::GetDescription() const {
    return description_;
}


const float &glimmer::Tile::GetHardness() const {
    return hardness_;
}

bool glimmer::Tile::IsBreakable() const {
    return breakable;
}

bool glimmer::Tile::IsOverwritable() const {
    return isOverwritable_;
}

bool glimmer::Tile::CanDropLoot() const {
    return canDropLoot_;
}

const std::string &glimmer::Tile::GetName() const {
    return name_;
}

glimmer::TileLayerType glimmer::Tile::GetLayerType() const {
    return layerType_;
}


TileVector2D glimmer::Tile::CalculateTileAnchor(const TileAnchorType tileAnchorType, const uint8_t tileWidth,
                                                const uint8_t tileHeight,
                                                const Vector2DIResource &customTileAnchor) {
    auto result = TileVector2D(1, 1);
    switch (tileAnchorType) {
        case TileAnchorType::TopLeft:
            result.x = 1;
            result.y = tileHeight;
            break;
        case TileAnchorType::Center:
            result.x = tileWidth / 2;
            result.y = tileHeight / 2;
            break;
        case TileAnchorType::CenterRight:
            result.x = tileWidth;
            result.y = tileHeight / 2;
            break;
        case TileAnchorType::TopCenter:
            result.x = tileWidth / 2;
            result.y = tileHeight;
            break;
        case TileAnchorType::TopRight:
            result.x = tileWidth;
            result.y = tileHeight;
            break;
        case TileAnchorType::CenterLeft:
            result.x = 1;
            result.y = tileHeight / 2;
            break;
        case TileAnchorType::BottomCenter:
            result.x = tileWidth / 2;
            result.y = 1;
            break;
        case TileAnchorType::BottomRight:
            result.x = tileWidth;
            result.y = 1;
            break;
        case TileAnchorType::Custom:
            result.x = customTileAnchor.x;
            result.y = customTileAnchor.y;
            break;
        case TileAnchorType::BottomLeft:
        default:
            result.x = 1;
            result.y = 1;
            break;
    }
    if (result.x < 1) {
        result.x = 1;
    }
    if (result.y < 1) {
        result.y = 1;
    }
    if (result.x > tileWidth) {
        result.x = tileWidth;
    }
    if (result.y > tileHeight) {
        result.y = tileHeight;
    }
    return result;
}

std::unique_ptr<glimmer::Tile> glimmer::Tile::FromTileResource(const AppContext *appContext,
                                                               const TileResource *tileResource) {
    if (appContext == nullptr) {
        return nullptr;
    }
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        return nullptr;
    }
    auto tile = std::make_unique<Tile>();
    tile->id_ = Resource::GenerateId(*tileResource);
    const StringResource *nameStringResource = resourceLocator->FindString(
        &tileResource->name);
    if (nameStringResource == nullptr) {
        tile->name_ = tile->id_;
    } else {
        tile->name_ = nameStringResource->value;
    }
    const StringResource *descriptionStringResource = resourceLocator->FindString(
        &tileResource->description);
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
    tile->backLightMask_ = tileResource->backLightMask;
    tile->sideLightMask_ = tileResource->sideLightMask;
    tile->lightSource_ = tileResource->lightSource;
    tile->isOverwritable_ = tileResource->isOverwritable;
    tile->canDropLoot_ = tileResource->canDropLoot;
    uint8_t tileHeight = tileResource->tileHeight;
    if (tileHeight > CHUNK_SIZE) {
        tileHeight = CHUNK_SIZE;
    }
    if (tileHeight == 0) {
        tileHeight = 1;
    }
    tile->tileHeight_ = tileHeight;
    uint8_t tileWidth = tileResource->tileWidth;
    if (tileWidth > CHUNK_SIZE) {
        tileWidth = CHUNK_SIZE;
    }
    if (tileWidth == 0) {
        tileWidth = 1;
    }
    tile->tileWidth_ = tileWidth;
    tile->tileAnchor_ = CalculateTileAnchor(static_cast<TileAnchorType>(tileResource->tileAnchorType), tileWidth,
                                            tileHeight,
                                            tileResource->customTileAnchor);
    tile->allowDirAdjustAnchor_ = tileResource->allowDirAdjustAnchor;
    tile->texture_ = resourceLocator->FindTexture(
        &tileResource->texture);
    tile->breakSFX_ = resourceLocator->FindAudio(&tileResource->breakSfx);
    tile->placeSFX_ = resourceLocator->FindAudio(&tileResource->placeSfx);
    return tile;
}

uint8_t glimmer::Tile::GetTileWidth() const {
    return tileWidth_;
}

uint8_t glimmer::Tile::GetTileHeight() const {
    return tileHeight_;
}

const TileVector2D *glimmer::Tile::GetTileAnchor() const {
    return &tileAnchor_;
}

const glimmer::ResourceRef *glimmer::Tile::GetLootTableRef() const {
    return &lootTable_;
}

const glimmer::ResourceRef *glimmer::Tile::GetLightSourceResource() const {
    return &lightSource_;
}
