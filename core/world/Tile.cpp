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
#include "WorldContext.h"
#include "core/scene/AppContext.h"
#include "core/mod/ResourceLocator.h"
#include "core/ecs/component/TechProviderComponent.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"

using enum glimmer::TileAnchorType;


glimmer::TilePhysicsType glimmer::Tile::GetTilePhysicsType() const
{
    return physicsType_;
}

const std::string& glimmer::Tile::GetId() const
{
    return id_;
}

bool glimmer::Tile::IsOverwritable() const
{
    return isOverwritable_;
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

const std::optional<std::string>& glimmer::Tile::GetDescription() const
{
    return description_;
}

glimmer::TileVector2D glimmer::Tile::CalculateTileAnchor(const TileAnchorType tileAnchorType, const uint8_t tileWidth,
                                                         const uint8_t tileHeight,
                                                         const Vector2DIResource& customTileAnchor)
{
    TileVector2D result = TileVector2D(0, 0);
    switch (tileAnchorType)
    {
    case TopLeft:
        result.x = 0;
        result.y = tileHeight - 1;
        break;
    case TopCenter:
        result.x = tileWidth / 2;
        result.y = tileHeight - 1;
        break;
    case TopRight:
        result.x = tileWidth - 1;
        result.y = tileHeight - 1;
        break;
    case CenterLeft:
        result.x = 0;
        result.y = tileHeight / 2;
        break;
    case Center:
        result.x = tileWidth / 2;
        result.y = tileHeight / 2;
        break;
    case CenterRight:
        result.x = tileWidth - 1;
        result.y = tileHeight / 2;
        break;
    case BottomLeft:
        result.x = 0;
        result.y = 0;
        break;
    case BottomCenter:
        result.x = tileWidth / 2;
        result.y = 0;
        break;
    case BottomRight:
        result.x = tileWidth - 1;
        result.y = 0;
        break;
    case Custom:
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
    tile->lootData_.SetCustomLootTable(tileResource->customLootTable);
    tile->lootData_.SetLootTable(tileResource->lootTable);
    tile->layerType_ = static_cast<TileLayerType>(tileResource->layerType);
    tile->physicsType_ = static_cast<TilePhysicsType>(tileResource->physicsType);
    tile->miningData_.SetAllowChainMining(tileResource->allowChainMining);
    tile->miningData_.SetMinMiningEfficiency(tileResource->minMiningEfficiency);
    tile->lightData_.SetBackLightMask(tileResource->backLightMask);
    tile->lightData_.SetSideLightMask(tileResource->sideLightMask);
    tile->lightData_.SetLightSource(tileResource->lightSource);
    tile->isOverwritable_ = tileResource->isOverwritable;
    tile->lootData_.SetCanDropLoot(tileResource->canDropLoot);
    uint8_t tileHeight = tileResource->tileHeight;
    if (tileHeight > CHUNK_SIZE)
    {
        tileHeight = CHUNK_SIZE;
    }
    if (tileHeight == 0)
    {
        tileHeight = 1;
    }
    tile->dimensions_.SetTileHeight(tileHeight);
    uint8_t tileWidth = tileResource->tileWidth;
    if (tileWidth > CHUNK_SIZE)
    {
        tileWidth = CHUNK_SIZE;
    }
    if (tileWidth == 0)
    {
        tileWidth = 1;
    }
    tile->dimensions_.SetTileWidth(tileWidth);
    if (tileResource->autoHardnessScale)
    {
        tile->miningData_.SetHardness(
            static_cast<float>(tileWidth) * static_cast<float>(tileHeight) * tileResource->unitHardness);
    }
    else
    {
        tile->miningData_.SetHardness(tileResource->unitHardness);
    }
    tile->technologyLevel_ = tileResource->technologyLevel;
    tile->recipeGroup_ = tileResource->recipeGroup;
    tile->dimensions_.SetTileAnchor(CalculateTileAnchor(static_cast<TileAnchorType>(tileResource->tileAnchorType),
                                                        tileWidth,
                                                        tileHeight,
                                                        tileResource->customTileAnchor));
    tile->dimensions_.SetAllowDirAdjustAnchor(tileResource->allowDirAdjustAnchor);
    const std::shared_ptr<TextureResourceResult>& textureResult = resourceLocator->FindTexture(
        &tileResource->texture);
    tile->resourceData_.SetTexture(textureResult);
    tile->blueprintData_.SetEnableBlueprint(tileResource->enableBlueprint);
    tile->blueprintData_.SetEnableBlueprintMask(tileResource->enableBlueprintMask);
    tile->blueprintData_.SetDrawValidBlueprintColor(tileResource->drawValidBlueprintColor);
    tile->blueprintData_.SetBlueprintTexture(resourceLocator->FindTextureRaw(
        &tileResource->blueprintTexture));
    if (tile->blueprintData_.GetBlueprintTexture() == nullptr)
    {
        tile->blueprintData_.SetBlueprintTexture(textureResult);
    }
    tile->lootData_.SetLootScaleBySize(tileResource->lootScaleBySize);
    tile->miningData_.SetUnitDigCost(tileResource->unitDigCost);
    tile->miningData_.SetAutoDigCostScale(tileResource->autoDigCostScale);
    tile->resourceData_.SetBreakSFX(resourceLocator->FindAudio(&tileResource->breakSfx));
    tile->resourceData_.SetPlaceSFX(resourceLocator->FindAudio(&tileResource->placeSfx));
    tile->resourceData_.SetTags(tileResource->tags);
    return tile;
}

void glimmer::Tile::OnPlace(const WorldContext* worldContext, PlaceSourceMessage placeSource,
                            const TileVector2D& position)
{
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return;
    }
    const Vector2DIFingerprint fingerprint = position.GetFingerprint();
    auto gameEntityIterator = gameEntities_.find(fingerprint);
    if (gameEntityIterator != gameEntities_.end())
    {
#if  !defined(NDEBUG)
        LogCat::e(
            "Before generating a new entity, it is necessary to ensure that there are no other entities at the current location.");
        assert(false);
#else
        return;
#endif
    }
    if (IsWorkBlock())
    {
        GameEntityID entity = entityManager->AddEntity();
        const auto transform2dComponent = entityManager->AddComponent<Transform2DComponent>(entity);
        transform2dComponent->SetPosition(CoordinateTransformer::TileToWorld(position));
        const auto teachProviderComponent = entityManager->AddComponent<TechProviderComponent>(entity);
        teachProviderComponent->SetRecipeGroup(static_cast<RecipeGroup>(recipeGroup_));
        teachProviderComponent->SetTechnologyLevel(technologyLevel_);
        gameEntities_[fingerprint] = entity;
    }
}

void glimmer::Tile::OnBreak(const WorldContext* worldContext, BreakSource breakSource, const TileVector2D& position)
{
    if (breakSource == BreakSource::ChunkLoad || breakSource == BreakSource::ChunkGenerate)
    {
        return;
    }
    EntityManager* entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr)
    {
        return;
    }
    const Vector2DIFingerprint fingerprint = position.GetFingerprint();
    auto gameEntityIterator = gameEntities_.find(fingerprint);
    if (gameEntityIterator == gameEntities_.end())
    {
        return;
    }
    entityManager->RemoveEntity(gameEntityIterator->second);
    gameEntities_.erase(gameEntityIterator);
    LogCat::d("Remove tile associated entities.");
}

glimmer::TileResourceData* glimmer::Tile::GetMutableResourceData()
{
    return &resourceData_;
}

const glimmer::TileResourceData* glimmer::Tile::GetResourceData() const
{
    return &resourceData_;
}

glimmer::TileBlueprintData* glimmer::Tile::GetMutableBlueprintData()
{
    return &blueprintData_;
}

const glimmer::TileBlueprintData* glimmer::Tile::GetBlueprintData() const
{
    return &blueprintData_;
}

glimmer::TileDimensions* glimmer::Tile::GetMutableDimensions()
{
    return &dimensions_;
}

const glimmer::TileDimensions* glimmer::Tile::GetDimensions() const
{
    return &dimensions_;
}

glimmer::TileMiningData* glimmer::Tile::GetMutableMiningData()
{
    return &miningData_;
}

const glimmer::TileMiningData* glimmer::Tile::GetMiningData() const
{
    return &miningData_;
}

glimmer::TileLootData* glimmer::Tile::GetMutableLootData()
{
    return &lootData_;
}

const glimmer::TileLootData* glimmer::Tile::GetLootData() const
{
    return &lootData_;
}

glimmer::TileLightResourceData* glimmer::Tile::GetMutableLightResourceData()
{
    return &lightData_;
}

const glimmer::TileLightResourceData* glimmer::Tile::GetLightResourceData() const
{
    return &lightData_;
}
