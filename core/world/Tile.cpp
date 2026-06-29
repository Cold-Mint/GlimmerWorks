/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "Tile.h"

#include <optional>

#include "TileAnchorType.h"
#include "../scene/AppContext.h"
#include "../mod/ResourceLocator.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/utils/StringUtils.h"


const glimmer::ResourceRef* glimmer::Tile::GetSideLightMaskResource() const
{
    return &sideLightMask_;
}

const glimmer::ResourceRef* glimmer::Tile::GetBackLightMaskResource() const
{
    return &backLightMask_;
}

bool glimmer::Tile::IsCustomLootTable() const
{
    return customLootTable_;
}

glimmer::TilePhysicsType glimmer::Tile::GetTilePhysicsType() const
{
    return physicsType_;
}

bool glimmer::Tile::IsAllowChainMining() const
{
    return allowChainMining_;
}

bool glimmer::Tile::IsAllowDirAdjustAnchor() const
{
    return allowDirAdjustAnchor_;
}

bool glimmer::Tile::LootScaleBySize() const
{
    return lootScaleBySize_;
}

const std::string& glimmer::Tile::GetId() const
{
    return id_;
}

SDL_Texture* glimmer::Tile::GetTexture() const
{
    if (textureResult_ == nullptr)
    {
        return nullptr;
    }
    return textureResult_->GetResource();
}

SDL_Texture* glimmer::Tile::GetBlueprintTexture() const
{
    if (blueprintTextureResult_ == nullptr)
    {
        return nullptr;
    }
    return blueprintTextureResult_->GetResource();
}

bool glimmer::Tile::EnableBlueprint() const
{
    return enableBlueprint_;
}

bool glimmer::Tile::EnableBlueprintMask() const
{
    return enableBlueprintMask_;
}

bool glimmer::Tile::DrawValidBlueprintColor() const
{
    return drawValidBlueprintColor_;
}

MIX_Audio* glimmer::Tile::GetBreakSFX() const
{
    if (breakSFXResult_ == nullptr)
    {
        return nullptr;
    }
    return breakSFXResult_->GetResource();
}

MIX_Audio* glimmer::Tile::GetPlaceSFX() const
{
    if (placeSFXResult_ == nullptr)
    {
        return nullptr;
    }
    return placeSFXResult_->GetResource();
}


const std::optional<std::string>& glimmer::Tile::GetDescription() const
{
    return description_;
}


const float& glimmer::Tile::GetHardness() const
{
    return hardness_;
}

bool glimmer::Tile::IsBreakable() const
{
    return hardness_ >= 0;
}

bool glimmer::Tile::IsOverwritable() const
{
    return isOverwritable_;
}

bool glimmer::Tile::CanDropLoot() const
{
    return canDropLoot_;
}

bool glimmer::Tile::IsWorkBlock() const
{
    return technologyLevel_ > 0;
}

const std::string& glimmer::Tile::GetName() const
{
    return name_;
}

glimmer::TileLayerType glimmer::Tile::GetLayerType() const
{
    return layerType_;
}


glimmer::TileVector2D glimmer::Tile::CalculateTileAnchor(const TileAnchorType tileAnchorType, const uint8_t tileWidth,
                                                         const uint8_t tileHeight,
                                                         const Vector2DIResource& customTileAnchor)
{
    TileVector2D result = TileVector2D(0, 0);
    switch (tileAnchorType)
    {
    case TileAnchorType::TopLeft:
        result.x = 0;
        result.y = tileHeight - 1;
        break;
    case TileAnchorType::TopCenter:
        result.x = tileWidth >> 1;
        result.y = tileHeight - 1;
        break;
    case TileAnchorType::TopRight:
        result.x = tileWidth - 1;
        result.y = tileHeight - 1;
        break;
    case TileAnchorType::CenterLeft:
        result.x = 0;
        result.y = tileHeight >> 1;
        break;
    case TileAnchorType::Center:
        result.x = tileWidth >> 1;
        result.y = tileHeight >> 1;
        break;
    case TileAnchorType::CenterRight:
        result.x = tileWidth - 1;
        result.y = tileHeight >> 1;
        break;
    case TileAnchorType::BottomLeft:
        result.x = 0;
        result.y = 0;
        break;
    case TileAnchorType::BottomCenter:
        result.x = tileWidth >> 1;
        result.y = 0;
        break;
    case TileAnchorType::BottomRight:
        result.x = tileWidth - 1;
        result.y = 0;
        break;
    case TileAnchorType::Custom:
        result.x = customTileAnchor.x;
        result.y = customTileAnchor.y;
        break;
    default:
        result.x = 0;
        result.y = tileHeight - 1;
        break;
    }

    if (result.x < 0)
    {
        result.x = 0;
    }
    if (result.y < 0)
    {
        result.y = 0;
    }
    if (result.x >= tileWidth)
    {
        result.x = tileWidth - 1;
    }
    if (result.y >= tileHeight)
    {
        result.y = tileHeight - 1;
    }
    return result;
}

std::unique_ptr<glimmer::Tile> glimmer::Tile::FromTileResource(const AppContext* appContext,
                                                               const TileResource* tileResource)
{
    if (appContext == nullptr)
    {
        return nullptr;
    }
    const ResourceLocator* resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr)
    {
        return nullptr;
    }
    auto tile = std::make_unique<Tile>();
    tile->id_ = Resource::GenerateId(*tileResource);
    const StringResource* nameStringResource = resourceLocator->FindString(
        &tileResource->name);
    if (nameStringResource == nullptr)
    {
        tile->name_ = tile->id_;
    }
    else
    {
        tile->name_ = nameStringResource->value;
    }
    const StringResource* descriptionStringResource = resourceLocator->FindString(
        &tileResource->description);
    if (descriptionStringResource != nullptr)
    {
        tile->description_ = descriptionStringResource->value;
    }
    tile->customLootTable_ = tileResource->customLootTable;
    tile->lootTable_ = tileResource->lootTable;
    tile->layerType_ = static_cast<TileLayerType>(tileResource->layerType);
    tile->physicsType_ = static_cast<TilePhysicsType>(tileResource->physicsType);
    tile->allowChainMining_ = tileResource->allowChainMining;
    tile->backLightMask_ = tileResource->backLightMask;
    tile->sideLightMask_ = tileResource->sideLightMask;
    tile->lightSource_ = tileResource->lightSource;
    tile->isOverwritable_ = tileResource->isOverwritable;
    tile->canDropLoot_ = tileResource->canDropLoot;
    uint8_t tileHeight = tileResource->tileHeight;
    if (tileHeight > CHUNK_SIZE)
    {
        tileHeight = CHUNK_SIZE;
    }
    if (tileHeight == 0)
    {
        tileHeight = 1;
    }
    tile->tileHeight_ = tileHeight;
    uint8_t tileWidth = tileResource->tileWidth;
    if (tileWidth > CHUNK_SIZE)
    {
        tileWidth = CHUNK_SIZE;
    }
    if (tileWidth == 0)
    {
        tileWidth = 1;
    }
    tile->tileWidth_ = tileWidth;
    if (tileResource->autoHardnessScale)
    {
        tile->hardness_ = static_cast<float>(tileWidth) * static_cast<float>(tileHeight) * tileResource->unitHardness;
    }
    else
    {
        tile->hardness_ = tileResource->unitHardness;
    }
    tile->technologyLevel_ = tileResource->technologyLevel;
    tile->recipeGroup_ = tileResource->recipeGroup;
    tile->tileAnchor_ = CalculateTileAnchor(static_cast<TileAnchorType>(tileResource->tileAnchorType), tileWidth,
                                            tileHeight,
                                            tileResource->customTileAnchor);
    tile->allowDirAdjustAnchor_ = tileResource->allowDirAdjustAnchor;
    tile->textureResult_ = resourceLocator->FindTexture(
        &tileResource->texture);
    tile->enableBlueprint_ = tileResource->enableBlueprint;
    tile->enableBlueprintMask_ = tileResource->enableBlueprintMask;
    tile->drawValidBlueprintColor_ = tileResource->drawValidBlueprintColor;
    tile->blueprintTextureResult_ = resourceLocator->FindTextureRaw(
        &tileResource->blueprintTexture);
    if (tile->blueprintTextureResult_ == nullptr)
    {
        tile->blueprintTextureResult_ = tile->textureResult_;
    }
    tile->lootScaleBySize_ = tileResource->lootScaleBySize;
    tile->unitDigCost_ = tileResource->unitDigCost;
    tile->autoDigCostScale_ = tileResource->autoDigCostScale;
    tile->breakSFXResult_ = resourceLocator->FindAudio(&tileResource->breakSfx);
    tile->placeSFXResult_ = resourceLocator->FindAudio(&tileResource->placeSfx);
    tile->tags_ = tileResource->tags;
    return tile;
}

uint8_t glimmer::Tile::GetTileWidth() const
{
    return tileWidth_;
}

const std::vector<glimmer::ItemTagResource>& glimmer::Tile::GetTags() const
{
    return tags_;
}

uint8_t glimmer::Tile::GetTileHeight() const
{
    return tileHeight_;
}

uint32_t glimmer::Tile::GetUnitDigCost() const
{
    return unitDigCost_;
}

bool glimmer::Tile::IsAutoDigCostScale() const
{
    return autoDigCostScale_;
}

const glimmer::TileVector2D* glimmer::Tile::GetTileAnchor() const
{
    return &tileAnchor_;
}

const glimmer::ResourceRef* glimmer::Tile::GetLootTableRef() const
{
    return &lootTable_;
}

const glimmer::ResourceRef* glimmer::Tile::GetLightSourceResource() const
{
    return &lightSource_;
}
