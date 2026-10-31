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
#include <unordered_set>

#include "TileAnchorType.h"
#include "TileAttachmentType.h"
#include "WorldContext.h"
#include "core/config/Constants.h"
#include "core/context/AppContext.h"
#include "core/context/CacheContext.h"
#include "core/mod/ResourceLocator.h"
#include "core/ecs/component/TechProviderComponent.h"
#include "core/ecs/component/CropComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/log/LogCat.h"
#include "core/math/CoordinateTransformer.h"
#include "core/math/WorldAABB.h"
#include "core/utils/StringUtils.h"

using enum glimmer::TileAnchorType;


const std::string &glimmer::Tile::GetId() const {
    return id_;
}

bool glimmer::Tile::IsOverwritable() const {
    return isOverwritable_;
}

bool glimmer::Tile::DestroySelfAfterGrowth() const {
    return destroySelfAfterGrowth_;
}

const glimmer::ResourceRef &glimmer::Tile::GetGrowthTarget() const {
    return growthTarget_;
}

const glimmer::ResourceRef &glimmer::Tile::GetPrimaryActionTarget() const {
    return primaryActionTarget_;
}

const glimmer::ResourceRef &glimmer::Tile::GetSecondaryActionTarget() const {
    return secondaryActionTarget_;
}

const glimmer::ResourceRef &glimmer::Tile::GetContainerLootTable() const {
    return containerLootTable_;
}

const glimmer::Vector2DIResource &glimmer::Tile::GetSpawnOriginOffset() const {
    return spawnOriginOffset_;
}

bool glimmer::Tile::AllowOfflineGrowth() const {
    return allowOfflineGrowth_;
}

bool glimmer::Tile::IsWorkBlock() const {
    return technologyLevel_ > 0;
}

bool glimmer::Tile::IsStorageBlock() const {
    return containerCapacity_ > 0;
}

uint8_t glimmer::Tile::GetContainerCapacity() const {
    return containerCapacity_;
}

uint16_t glimmer::Tile::GetContainerMaxStack() const {
    return containerMaxStack_;
}

GameEntityID glimmer::Tile::GetGameEntity(const TileVector2D &position) const {
    const auto iterator = gameEntities_.find(position.GetFingerprint());
    if (iterator == gameEntities_.end()) {
        return GAME_ENTITY_ID_INVALID;
    }
    return iterator->second;
}

bool glimmer::Tile::IsCropsBlock() const {
    return IsCropsBlock(growthMinTicks_, growthTarget_);
}

bool glimmer::Tile::IsCropsBlock(const uint64_t growthMinTicks, const ResourceRef &growthTarget) {
    return growthMinTicks > 0 && growthTarget.IsValid();
}

const std::string &glimmer::Tile::GetName() const {
    return name_;
}

glimmer::TileLayerType glimmer::Tile::GetLayerType() const {
    return layerType_;
}

const std::vector<glimmer::ResourceRef> &glimmer::Tile::GetGrowthConditions() const {
    return growthConditions_;
}

const std::optional<std::string> &glimmer::Tile::GetDescription() const {
    return description_;
}

glimmer::TileVector2D glimmer::Tile::CalculateTileAnchor(const TileAnchorType tileAnchorType, const uint8_t tileWidth,
                                                         const uint8_t tileHeight,
                                                         const Vector2DIResource &customTileAnchor) {
    TileVector2D result = TileVector2D(0, 0);
    switch (tileAnchorType) {
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

    if (result.x < 0) {
        result.x = 0;
    }
    if (result.y < 0) {
        result.y = 0;
    }
    if (result.x >= tileWidth) {
        result.x = tileWidth - 1;
    }
    if (result.y >= tileHeight) {
        result.y = tileHeight - 1;
    }
    return result;
}

void glimmer::Tile::AppendDerivedTag(std::unordered_set<uint64_t> &existingTagIds, std::vector<ItemTagResource> &tags,
                                     const std::string &name) {
    ItemTagResource tag = StringUtils::MakeTag(name);
    tag.MakeCachedTag();
    if (existingTagIds.contains(tag.cachedTagId)) {
        return;
    }
    existingTagIds.insert(tag.cachedTagId);
    tags.emplace_back(tag);
}

std::unique_ptr<glimmer::Tile> glimmer::Tile::FromTileResource(const AppContext *appContext,
                                                               const TileResource *tileResource) {
    if (appContext == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "tile_from_resource_null_context",
                  "Cannot create tile: app context is null");
        return nullptr;
    }
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "tile_from_resource_null_locator",
                  "Cannot create tile: resource locator is null");
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
    tile->lootData_.SetCustomLootTable(tileResource->customLootTable);
    tile->lootData_.SetLootTable(tileResource->lootTable);
    tile->layerType_ = static_cast<TileLayerType>(tileResource->layer);
    tile->collisionMask_ = tileResource->collisionMask;
    tile->collisionBoxSize_ = tileResource->collisionBoxSize;
    tile->collisionBoxOffset_ = tileResource->collisionBoxOffset;
    tile->miningData_.SetAllowChainMining(tileResource->allowChainMining);
    tile->miningData_.SetMinMiningEfficiency(tileResource->minMiningEfficiency);
    tile->lightData_.SetBackLightMask(tileResource->backLightMask);
    tile->lightData_.SetSideLightMask(tileResource->sideLightMask);
    tile->lightData_.SetLightSource(tileResource->lightSource);
    tile->isOverwritable_ = tileResource->isOverwritable;
    tile->lootData_.SetCanDropLoot(tileResource->canDropLoot);
    tile->growthMinTicks_ = tileResource->growthMinTicks;
    tile->growthMaxTicks_ = tileResource->growthMaxTicks;
    tile->growthTarget_ = tileResource->growthTarget;
    tile->destroySelfAfterGrowth_ = tileResource->destroySelfAfterGrowth;
    tile->growthConditions_ = tileResource->growthConditions;
    tile->spawnOriginOffset_ = tileResource->spawnOriginOffset;
    tile->allowOfflineGrowth_ = tileResource->allowOfflineGrowth;
    tile->primaryActionTarget_ = tileResource->primaryActionTarget;
    tile->secondaryActionTarget_ = tileResource->secondaryActionTarget;
    tile->containerLootTable_ = tileResource->containerLootTable;
    uint8_t tileHeight = tileResource->tileHeight;
    if (tileHeight > CHUNK_SIZE) {
        tileHeight = CHUNK_SIZE;
    }
    if (tileHeight == 0) {
        tileHeight = 1;
    }
    tile->dimensions_.SetTileHeight(tileHeight);
    uint8_t tileWidth = tileResource->tileWidth;
    if (tileWidth > CHUNK_SIZE) {
        tileWidth = CHUNK_SIZE;
    }
    if (tileWidth == 0) {
        tileWidth = 1;
    }
    tile->dimensions_.SetTileWidth(tileWidth);
    if (tileResource->autoHardnessScale) {
        tile->miningData_.SetHardness(
            static_cast<float>(tileWidth) * static_cast<float>(tileHeight) * tileResource->unitHardness);
    } else {
        tile->miningData_.SetHardness(tileResource->unitHardness);
    }
    tile->technologyLevel_ = tileResource->technologyLevel;
    tile->recipeGroup_ = tileResource->recipeGroup;
    tile->containerCapacity_ = tileResource->containerCapacity;
    tile->containerMaxStack_ = tileResource->containerMaxStack;
    tile->dimensions_.SetTileAnchor(CalculateTileAnchor(static_cast<TileAnchorType>(tileResource->tileAnchorType),
                                                        tileWidth,
                                                        tileHeight,
                                                        tileResource->customTileAnchor));
    tile->dimensions_.SetAllowDirAdjustAnchor(tileResource->allowDirAdjustAnchor);
    const std::shared_ptr<TextureResourceResult> &textureResult = resourceLocator->FindTexture(
        &tileResource->texture);
    tile->resourceData_.SetTextureRef(tileResource->texture);
    tile->resourceData_.SetTexture(textureResult);
    if (tileResource->attachmentTextureUp.IsValid()) {
        tile->resourceData_.SetAttachmentTexture(TileAttachmentDirection::Up,
                                                 resourceLocator->FindTexture(&tileResource->attachmentTextureUp));
    }
    if (tileResource->attachmentTextureRight.IsValid()) {
        tile->resourceData_.SetAttachmentTexture(TileAttachmentDirection::Right,
                                                 resourceLocator->FindTexture(&tileResource->attachmentTextureRight));
    }
    if (tileResource->attachmentTextureDown.IsValid()) {
        tile->resourceData_.SetAttachmentTexture(TileAttachmentDirection::Down,
                                                 resourceLocator->FindTexture(&tileResource->attachmentTextureDown));
    }
    if (tileResource->attachmentTextureLeft.IsValid()) {
        tile->resourceData_.SetAttachmentTexture(TileAttachmentDirection::Left,
                                                 resourceLocator->FindTexture(&tileResource->attachmentTextureLeft));
    }
    if (tileResource->attachmentTextureBack.IsValid()) {
        tile->resourceData_.SetAttachmentTexture(TileAttachmentDirection::BackLayer,
                                                 resourceLocator->FindTexture(&tileResource->attachmentTextureBack));
    }
    if (tileResource->pipeline.IsValid()) {
        const std::shared_ptr<GPUPipelineResourceResult> pipelineResult = resourceLocator->FindGPUGraphicsPipeline(
            &tileResource->pipeline, false);
        if (pipelineResult != nullptr) {
            tile->resourceData_.SetPipeline(pipelineResult);
        }
    }
    if (tileResource->sampler.IsValid()) {
        const std::shared_ptr<GPUSamplerResourceResult> samplerResult = resourceLocator->FindGPUGraphicsSampler(
            &tileResource->sampler, false);
        if (samplerResult != nullptr && samplerResult->GetResource() != nullptr) {
            tile->resourceData_.SetSampler(samplerResult);
        }
    }
    tile->blueprintData_.SetEnableBlueprint(tileResource->enableBlueprint);
    tile->blueprintData_.SetEnableBlueprintMask(tileResource->enableBlueprintMask);
    tile->blueprintData_.SetDrawValidBlueprintColor(tileResource->drawValidBlueprintColor);
    tile->lootData_.SetLootScaleBySize(tileResource->lootScaleBySize);
    tile->miningData_.SetUnitDigCost(tileResource->unitDigCost);
    tile->miningData_.SetAutoDigCostScale(tileResource->autoDigCostScale);
    if (tileResource->breakSfx.IsValid()) {
        tile->resourceData_.SetBreakSFX(resourceLocator->FindAudio(&tileResource->breakSfx));
    }
    if (tileResource->placeSfx.IsValid()) {
        tile->resourceData_.SetPlaceSFX(resourceLocator->FindAudio(&tileResource->placeSfx));
    }
    if (tileResource->primaryActionSfx.IsValid()) {
        tile->resourceData_.SetPrimaryActionSFX(resourceLocator->FindAudio(&tileResource->primaryActionSfx));
    }
    if (tileResource->secondaryActionSfx.IsValid()) {
        tile->resourceData_.SetSecondaryActionSFX(resourceLocator->FindAudio(&tileResource->secondaryActionSfx));
    }
    std::vector<ItemTagResource> tags = tileResource->tags;

    if (tileResource->autoDerivedTags) {
        const uint64_t voidTag = StringUtils::StringToUint64(TAG_VOID);
        const uint64_t airTag = StringUtils::StringToUint64(TAG_AIR);
        std::unordered_set<uint64_t> existingTagIds;
        for (const auto &tag: tags) {
            existingTagIds.insert(tag.cachedTagId);
        }
        if (tile->isOverwritable_) {
            AppendDerivedTag(existingTagIds, tags, TAG_REPLACEABLE);
        }
        if (!tile->miningData_.IsBreakable() && !tile->isOverwritable_) {
            AppendDerivedTag(existingTagIds, tags, TAG_UNBREAKABLE);
        }
        if (tileResource->partial) {
            AppendDerivedTag(existingTagIds, tags, TAG_PARTIAL);
        }
        if (tileResource->solid) {
            AppendDerivedTag(existingTagIds, tags, TAG_SOLID);
        }
        //Prohibit the derivation of tiles with the "void" or "air" labels.
        //禁止对带有void或air标签的瓦片派生
        if (!existingTagIds.contains(airTag) && !existingTagIds.contains(voidTag)) {
            if (tile->layerType_ == TileLayerType::BackGround) {
                AppendDerivedTag(existingTagIds, tags, TAG_WALL);
            }
        }
    }
    tile->resourceData_.SetTags(tags);
    tile->attachmentMask_ = tileResource->attachmentMask;
    tile->attachmentTagIds_ = tileResource->cachedAttachmentTagIds;
    return tile;
}

uint8_t glimmer::Tile::GetCollisionMask() const {
    return collisionMask_;
}

const glimmer::Vector2DResource &glimmer::Tile::GetCollisionBoxSize() const {
    return collisionBoxSize_;
}

const glimmer::Vector2DResource &glimmer::Tile::GetCollisionBoxOffset() const {
    return collisionBoxOffset_;
}

glimmer::WorldAABB glimmer::Tile::GetCollisionAABB(const TileVector2D &tilePos) const {
    const float cellMinX = static_cast<float>(tilePos.x * TILE_SIZE) - static_cast<float>(HALF_TILE_SIZE);
    const float cellMinY = static_cast<float>(tilePos.y * TILE_SIZE) - static_cast<float>(HALF_TILE_SIZE);
    const float boxLeft = cellMinX + collisionBoxOffset_.x * static_cast<float>(TILE_SIZE);
    const float boxBottom = cellMinY + collisionBoxOffset_.y * static_cast<float>(TILE_SIZE);
    const float boxRight = boxLeft + collisionBoxSize_.x * static_cast<float>(TILE_SIZE);
    const float boxTop = boxBottom + collisionBoxSize_.y * static_cast<float>(TILE_SIZE);

    WorldAABB box;
    box.center = WorldVector2D{(boxLeft + boxRight) * 0.5F, (boxBottom + boxTop) * 0.5F};
    box.halfWidth = (boxRight - boxLeft) * 0.5F;
    box.halfHeight = (boxTop - boxBottom) * 0.5F;
    return box;
}

void glimmer::Tile::OnPlace(WorldContext *worldContext, PlaceSourceMessage placeSource,
                            const TileVector2D &position, const TileStateMessage *tileState) {
    EntityManager *entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr || tileState == nullptr) {
        return;
    }
    //Block-level entities (work blocks / crops / storage blocks) are created once per block, anchored
    //at the block's bottom-left origin cell. The stored offset is "top-left - cell" = (-cellX, cellY),
    //so the bottom-left origin cell satisfies cellX == 0 and cellY == height - 1. Other cells of a
    //multi-tile block must not create an entity, otherwise a 2-tall storage block would get two
    //duplicated containers.
    //方块级实体（工作方块/作物/存储方块）每个方块只建一个，锚定在左下角原点格。
    //存储的 offset 为「左上角 - 当前格」=(-cellX, cellY)，左下角格满足 cellX==0、cellY==height-1。
    //多格方块的其他格子不应再建实体，否则 2 格存储方块会得到两个重复的容器。
    TileVector2D offset(0, 0);
    offset.ReadVector2DIMessage(tileState->offset());
    if (offset.x != 0 || offset.y != static_cast<int>(tileState->height()) - 1) {
        return;
    }
    const Vector2DIFingerprint fingerprint = position.GetFingerprint();
    auto gameEntityIterator = gameEntities_.find(fingerprint);
    if (gameEntityIterator != gameEntities_.end()) {
#if  !defined(NDEBUG)
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "tile_place_entity_exists",
                  "Before generating a new entity, it is necessary to ensure that there are no other entities at the current location.");
        assert(false);
#else
        return;
#endif
    }
    GameEntityID entity = GAME_ENTITY_ID_INVALID;
    if (IsWorkBlock()) {
        if (entity == GAME_ENTITY_ID_INVALID) {
            entity = entityManager->AddEntity();
        }
        const auto teachProviderComponent = entityManager->AddComponent<TechProviderComponent>(entity);
        teachProviderComponent->SetRecipeGroup(static_cast<RecipeGroup>(recipeGroup_));
        teachProviderComponent->SetTechnologyLevel(technologyLevel_);
        LogCat::d(LogLabel::DEFAULT, "tile_on_place_entity_created",
                  "Placed work block, created entity: id={}, position=({}, {})",
                  entity, position.x, position.y);
    }
    if (IsCropsBlock()) {
        if (entity == GAME_ENTITY_ID_INVALID) {
            entity = entityManager->AddEntity();
        }
        const auto cropComponent = entityManager->AddComponent<CropComponent>(entity);
        cropComponent->SetPosition(position);
        cropComponent->SetLayerType(layerType_);
        LogCat::d(LogLabel::DEFAULT, "tile_on_place_entity_created",
                  "Placed crop block, created entity: id={}, position=({}, {})",
                  entity, position.x, position.y);
    }
    if (IsStorageBlock()) {
        if (entity == GAME_ENTITY_ID_INVALID) {
            entity = entityManager->AddEntity();
        }
        const auto itemContainerComponent = entityManager->AddComponent<ItemContainerComponent>(entity);
        ItemContainer *itemContainer = itemContainerComponent->GetItemContainer();
        if (itemContainer != nullptr) {
            itemContainer->Resize(containerCapacity_);
            itemContainer->SetMaxStackPerSlot(containerMaxStack_);
            if (tileState->has_itemcontainer()) {
                itemContainer->FromMessage(worldContext, tileState->itemcontainer());
            }
        }
        LogCat::d(LogLabel::DEFAULT, "tile_on_place_entity_created",
                  "Placed storage block, created entity: id={}, position=({}, {})",
                  entity, position.x, position.y);
    }
    if (entity == GAME_ENTITY_ID_INVALID) {
        return;
    }
    const auto transform2dComponent = entityManager->AddComponent<Transform2DComponent>(entity);
    transform2dComponent->SetPosition(CoordinateTransformer::TileToWorld(position));
    gameEntities_[fingerprint] = entity;
}

void glimmer::Tile::OnBreak(WorldContext *worldContext, const BreakSource breakSource,
                            const TileVector2D &position) {
    if (breakSource == BreakSource::ChunkLoad || breakSource == BreakSource::ChunkGenerate) {
        return;
    }
    EntityManager *entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr) {
        return;
    }
    const Vector2DIFingerprint fingerprint = position.GetFingerprint();
    auto gameEntityIterator = gameEntities_.find(fingerprint);
    if (gameEntityIterator == gameEntities_.end()) {
        return;
    }
    const GameEntityID entityId = gameEntityIterator->second;
    if (IsStorageBlock() && breakSource != BreakSource::ChunkUnload) {
        const auto itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(entityId);
        if (itemContainerComponent != nullptr) {
            ItemContainer *itemContainer = itemContainerComponent->GetItemContainer();
            if (itemContainer != nullptr) {
                const WorldVector2D dropPosition = CoordinateTransformer::TileToWorld(position);
                for (uint8_t i = 0; i < itemContainer->GetCapacity(); ++i) {
                    ItemResult itemResult = itemContainer->TakeAllItem(i);
                    if (!itemResult.IsSuccess() || itemResult.GetItem() == nullptr) {
                        continue;
                    }
                    DroppedItemCreator::SpawnDroppedItem(worldContext, dropPosition, itemResult.TakeItem(),
                                                         DROPPED_ITEM_PICKUP_COOLDOWN_TICKS);
                }
            }
        }
    }
    entityManager->RemoveEntity(entityId);
    LogCat::d(LogLabel::DEFAULT, "tile_on_break_entity_removed",
              "Broke block, removed entity: id={}, position=({}, {})",
              entityId, position.x, position.y);
    gameEntities_.erase(gameEntityIterator);
}

const glimmer::TileResourceData &glimmer::Tile::GetResourceData() const {
    return resourceData_;
}

glimmer::TileBlueprintData &glimmer::Tile::GetMutableBlueprintData() {
    return blueprintData_;
}

const glimmer::TileBlueprintData &glimmer::Tile::GetBlueprintData() const {
    return blueprintData_;
}

glimmer::TileDimensions &glimmer::Tile::GetMutableDimensions() {
    return dimensions_;
}

const glimmer::TileDimensions &glimmer::Tile::GetDimensions() const {
    return dimensions_;
}

glimmer::TileMiningData &glimmer::Tile::GetMutableMiningData() {
    return miningData_;
}

const glimmer::TileMiningData &glimmer::Tile::GetMiningData() const {
    return miningData_;
}

glimmer::TileLootData &glimmer::Tile::GetMutableLootData() {
    return lootData_;
}

const glimmer::TileLootData &glimmer::Tile::GetLootData() const {
    return lootData_;
}

glimmer::TileLightResourceData *glimmer::Tile::GetMutableLightResourceData() {
    return &lightData_;
}

const glimmer::TileLightResourceData *glimmer::Tile::GetLightResourceData() const {
    return &lightData_;
}

uint8_t glimmer::Tile::GetAttachmentMask() const {
    return attachmentMask_;
}

const std::vector<uint64_t> &glimmer::Tile::GetAttachmentTagIds() const {
    return attachmentTagIds_;
}
