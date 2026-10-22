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
#include "TileItem.h"

#include "ItemUseContext.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "core/ecs/component/TileLayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/config/Constants.h"

#include "core/ecs/system/MiningSystem.h"


glimmer::TileItem::TileItem(const std::shared_ptr<Tile> &tile, const ResourceRef &resourceRef) : tile_(tile) {
    SetResourceRef(resourceRef);
    const TileResourceData &tileResourceData = tile_->GetResourceData();
    SetTags(tileResourceData.GetTags());
    SetMaxStack(ITEM_MAX_STACK);
    SetUnbreakable(true);
    const TileLightResourceData *tileLightResourceData = tile->GetLightResourceData();
    if (tileLightResourceData != nullptr) {
        if (const ResourceRef *lightResourceRef = tileLightResourceData->GetLightSourceResource(); lightResourceRef->
            IsValid()) {
            SetLightSourceRef(*lightResourceRef);
        }
    }
}

const std::string &glimmer::TileItem::GetId() const {
    return tile_->GetId();
}

const std::string &glimmer::TileItem::GetName() const {
    return tile_->GetName();
}

const std::optional<std::string> &glimmer::TileItem::GetDescription() const {
    return tile_->GetDescription();
}

const glimmer::Tile *glimmer::TileItem::GetTile() const {
    return tile_.get();
}

bool glimmer::TileItem::OnUse(const ItemUseContext &context) {
    const bool mouseRightDown = context.IsMouseRightDown();
    if (!mouseRightDown) {
        return false;
    }
    if (tile_ == nullptr) {
        return false;
    }
    WorldContext *worldContext = context.worldContext;
    if (worldContext == nullptr) {
        return false;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return false;
    }
    auto playerEntity = entityShortCut->GetPlayer();
    if (WorldContext::IsEmptyEntityId(playerEntity)) {
        return false;
    }
    EntityManager *entityManager = worldContext->GetEntityManager();
    auto playerTransform = entityManager->GetComponent<Transform2DComponent>(playerEntity);
    if (playerTransform == nullptr) {
        return false;
    }
    const BlueprintComponent *blueprintComponent = entityShortCut->GetBlueprintComponent();
    if (blueprintComponent == nullptr) {
        return false;
    }
    ItemStackModule *itemStackModule = GetMutableStackModule();
    if (itemStackModule == nullptr) {
        return false;
    }
    const auto entities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
    const TileLayerType targetTileLayerType = tile_->GetLayerType();
    for (auto &entity: entities) {
        auto tileLayer = entityManager->GetComponent<TileLayerComponent>(entity);
        if (tileLayer == nullptr) {
            continue;
        }
        if (tileLayer->GetTileLayerType() != targetTileLayerType) {
            //The tile layer is incorrect. Let's look for the next one.
            //瓦片图层不对，找下一个。
            continue;
        }
        if (!blueprintComponent->CanPlace()) {
            continue;
        }
        if (itemStackModule->GetAmount() > 0) {
            if (AudioManager *audioManager = appContext->GetAudioContext()->GetAudioManager();
                audioManager != nullptr) {
                if (const AudioResourceResult *audioResourceResult = tile_->GetResourceData().GetPlaceSFX();
                    audioResourceResult !=
                    nullptr) {
                    audioManager->TryPlayFree(
                        AudioType::AMBIENT, audioResourceResult->GetResource(), 0);
                }
            }
            TileBreakParams tileBreakParams;
            tileBreakParams.SetBreakSource(BreakSource::PlayerOverride);
            tileBreakParams.SetWorldContext(worldContext);
            tileBreakParams.SetTileLayerComponent(tileLayer);
            tileBreakParams.SetTopLeftPosition(blueprintComponent->GetTopLeftVector());
            tileBreakParams.SetPrecisionMining(false);
            tileBreakParams.SetPlaceMode(true);
            const TileDimensions &tileDimensions = tile_->GetDimensions();
            tileBreakParams.SetTileWidth(tileDimensions.GetTileWidth());
            tileBreakParams.SetTileHeight(tileDimensions.GetTileHeight());
            tileBreakParams.SetNewTileRef(GetResourceRef());
            tileBreakParams.SetAttachmentDirection(blueprintComponent->GetAttachmentDirection());
            tileBreakParams.SetAttachmentOffset(blueprintComponent->GetAttachmentOffset());
            MiningSystem::BreakTile(tileBreakParams);

            itemStackModule->RemoveAmount(1);
        }
    }
    return true;
}

glimmer::TextureResourceResult *glimmer::TileItem::GetIcon() const {
    if (tile_ == nullptr) {
        return nullptr;
    }
    return tile_->GetResourceData().GetTexture();
}

SDL_GPUGraphicsPipeline *glimmer::TileItem::GetPipeline() const {
    if (tile_ == nullptr) {
        return nullptr;
    }
    return tile_->GetResourceData().GetPipeline();
}

SDL_GPUSampler *glimmer::TileItem::GetSampler() const {
    if (tile_ == nullptr) {
        return nullptr;
    }
    return tile_->GetResourceData().GetSampler();
}


const glimmer::ResourceRef *glimmer::TileItem::GetIconResourceRef() const {
    if (tile_ == nullptr) {
        return nullptr;
    }
    return tile_->GetResourceData().GetTextureRef();
}

const glimmer::AbilityConfig *glimmer::TileItem::GetAbilityConfig() const {
    return nullptr;
}

std::unique_ptr<glimmer::Item> glimmer::TileItem::Clone() const {
    return std::make_unique<TileItem>(tile_, GetResourceRef());
}
