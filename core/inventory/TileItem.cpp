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
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include "../ecs/component/TileLayerComponent.h"
#include "../ecs/component/Transform2DComponent.h"
#include "../Constants.h"

#include "core/ecs/system/DiggingSystem.h"


glimmer::TileItem::TileItem(const std::shared_ptr<Tile>& tile, const ResourceRef& resourceRef) : tile_(tile)
{
    resourceRef_ = resourceRef;
    maxStack_ = ITEM_MAX_STACK;
}

const std::string& glimmer::TileItem::GetId() const
{
    return tile_->GetId();
}

const std::string& glimmer::TileItem::GetName() const
{
    return tile_->GetName();
}

const std::optional<std::string>& glimmer::TileItem::GetDescription() const
{
    return tile_->GetDescription();
}

const glimmer::Tile* glimmer::TileItem::GetTile() const
{
    return tile_.get();
}

uint32_t glimmer::TileItem::GetMaxDurability() const
{
    return 0;
}

bool glimmer::TileItem::IsUnbreakable() const
{
    return true;
}

void glimmer::TileItem::Reduce(unsigned value)
{
}

void glimmer::TileItem::OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                              std::unordered_set<std::string>& popupAbility)
{
    if (tile_ == nullptr)
    {
        return;
    }
    if (worldContext == nullptr)
    {
        return;
    }
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    auto playerEntity = worldContext->GetPlayerEntity();
    if (WorldContext::IsEmptyEntityId(playerEntity))
    {
        return;
    }
    auto playerTransform = worldContext->GetComponent<Transform2DComponent>(playerEntity);
    if (playerTransform == nullptr)
    {
        return;
    }
    const BlueprintComponent* blueprintComponent = worldContext->GetBlueprintComponent();
    if (blueprintComponent == nullptr)
    {
        return;
    }
    const auto entities = worldContext->GetEntityIDWithComponents<TileLayerComponent>();
    const TileLayerType targetTileLayerType = tile_->GetLayerType();
    for (auto& entity : entities)
    {
        auto* tileLayer = worldContext->GetComponent<TileLayerComponent>(entity);
        if (tileLayer == nullptr)
        {
            continue;
        }
        if (tileLayer->GetTileLayerType() != targetTileLayerType)
        {
            //The tile layer is incorrect. Let's look for the next one.
            //瓦片图层不对，找下一个。
            continue;
        }
        if (!blueprintComponent->CanPlace())
        {
            continue;
        }
        if (GetAmount() > 0)
        {
            AudioManager* audioManager = appContext->GetAudioManager();
            if (audioManager != nullptr)
            {
                audioManager->TryPlayFree(
                    AMBIENT, tile_->GetPlaceSFX(), 0);
            }
            DiggingSystem::BreakTile(worldContext, tileLayer, blueprintComponent->GetTopLeftVector(), false, true,
                                     tile_->GetTileWidth(), tile_->GetTileHeight(),
                                     resourceRef_);
            (void)RemoveAmount(1);
        }
    }
}


SDL_Texture* glimmer::TileItem::GetIcon() const
{
    return tile_->GetTexture();
}

const glimmer::AbilityConfig* glimmer::TileItem::GetAbilityConfig() const
{
    return nullptr;
}


std::unique_ptr<glimmer::Item> glimmer::TileItem::Clone() const
{
    return std::make_unique<TileItem>(tile_, resourceRef_);
}
