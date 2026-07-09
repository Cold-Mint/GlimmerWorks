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
#include "ResourceLocator.h"

#include "core/scene/AppContext.h"
#include "core/inventory/AbilityItem.h"
#include "core/inventory/ComposableItem.h"
#include "core/inventory/MaterialItem.h"
#include "core/inventory/TileItem.h"
#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "core/world/TileInstancePool.h"
#include "dataPack/StringManager.h"

bool glimmer::ResourceLocator::ValidateAccessPermission(const ResourceRef* resourceRef) const
{
    if (resourceRef->GetSelfPackageId() == resourceRef->GetPackageId())
    {
        //Allow access to one's own package.
        //允许访问自身包。
        return true;
    }

    bool result = appContext_->GetModContext()->GetDataPackManager()->IsDependencySatisfied(
        resourceRef->GetSelfPackageId(), resourceRef->GetPackageId());
    if (!result)
    {
        LogCat::w("Prevented access to resources. Source Package ID: ", resourceRef->GetSelfPackageId(),
                  ", Target Package ID: ", resourceRef->GetPackageId(), ".");
    }
    return result;
}

glimmer::ResourceLocator::ResourceLocator(AppContext* appContext_) : appContext_(appContext_)
{
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourceLocator::FindTexture(
    const ResourceRef* resourceRef) const
{
    std::shared_ptr<TextureResourceResult> result = FindTextureRaw(resourceRef);
    if (result == nullptr)
    {
        return appContext_->GetResourcePackManager()->errorTexture_;
    }
    return result;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourceLocator::FindTextureRaw(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TEXTURE)
    {
        return nullptr;
    }
    if (!ValidateAccessPermission(resourceRef))
    {
        return appContext_->GetResourcePackManager()->accessDeniedTexture_;
    }
    return appContext_->GetResourcePackManager()->LoadTextureFromFile(appContext_, resourceRef);
}

std::shared_ptr<glimmer::AudioResourceResult> glimmer::ResourceLocator::FindAudio(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_AUDIO || !
        ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetResourcePackManager()->LoadAudioFromFile(appContext_, resourceRef);
}

std::unique_ptr<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    const uint32_t resourceType = resourceRef->GetResourceType();
    if (!ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    if (resourceType == RESOURCE_COLOR)
    {
        const ColorResource* colorResource = appContext_->GetResourcePackManager()->LoadColorResFromFile(
            appContext_, resourceRef);
        if (colorResource == nullptr)
        {
            return nullptr;
        }
        return std::make_unique<Color>(colorResource->ToColor());
    }
    if (resourceType == RESOURCE_FIXED_COLOR)
    {
        const FixedColorResource* fixedColorResource = appContext_->GetGraphicsContext()->GetFixedColorManager()->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr)
        {
            return nullptr;
        }
        return std::make_unique<Color>(fixedColorResource->ToColor());
    }
    return nullptr;
}


glimmer::IShapeResource* glimmer::ResourceLocator::FindShape(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SHAPE || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetShapeManager()->FindShape(resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::IBiomeDecoratorResource* glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_BIOME_DECORATOR || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetBiomeDecoratorResourcesManager()->FindBiomeDecorator(
        resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::StringResource* glimmer::ResourceLocator::FindString(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_STRING || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetStringManager()->Find(resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::LightSourceResource* glimmer::ResourceLocator::FindLightSource(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_SOURCE || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetGraphicsContext()->GetLightSourceManager()->FindLightSourceResource(resourceRef->GetPackageId(),
                                                                         resourceRef->GetResourceKey());
}

glimmer::LightMaskResource* glimmer::ResourceLocator::FindLightMask(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_MASK || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetGraphicsContext()->GetLightMaskManager()->FindLightMaskResource(resourceRef->GetPackageId(),
                                                                     resourceRef->GetResourceKey());
}

glimmer::TileResource* glimmer::ResourceLocator::FindTileFallback(const ResourceRef* resourceRef,
                                                                  TileLayerType tileLayer) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef))
    {
        return appContext_->GetModContext()->GetTileResourceManager()->GenerateAccessDeniedPlaceHolder(
            resourceRef->GetPackageId(), resourceRef->GetResourceKey(), tileLayer);
    }
    return appContext_->GetModContext()->GetTileResourceManager()->FindTileFallback(resourceRef->GetPackageId(),
                                                                   resourceRef->GetResourceKey(),
                                                                   tileLayer);
}

glimmer::TileResource* glimmer::ResourceLocator::FindTileRaw(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetTileResourceManager()->FindTileRaw(resourceRef->GetPackageId(),
                                                              resourceRef->GetResourceKey());
}

glimmer::MobResource* glimmer::ResourceLocator::FindMob(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MOB || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetMobManager()->FindMobResource(resourceRef->GetPackageId(),
                                                         resourceRef->GetResourceKey());
}

glimmer::ComposableItemResource* glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_COMPOSABLE_ITEM || !
        ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetItemManager()->FindComposableItemResource(resourceRef->GetPackageId(),
                                                                     resourceRef->GetResourceKey());
}

glimmer::AbilityItemResource* glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_ABILITY_ITEM || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetItemManager()->FindAbilityItemResource(resourceRef->GetPackageId(),
                                                                  resourceRef->GetResourceKey());
}

glimmer::MaterialItemResource* glimmer::ResourceLocator::FindMaterialItem(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MATERIAL_ITEM || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetItemManager()->FindMaterialItemResource(resourceRef->GetPackageId(),
                                                                   resourceRef->GetResourceKey());
}

glimmer::LootResource* glimmer::ResourceLocator::FindLoot(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LOOT_TABLE || !ValidateAccessPermission(resourceRef))
    {
        return nullptr;
    }
    return appContext_->GetModContext()->GetLootTableManager()->Find(resourceRef->GetPackageId(),
                                                    resourceRef->GetResourceKey());
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext* worldContext,
                                                                  const ItemMessage& itemMessage) const
{
    if (worldContext == nullptr)
    {
        return nullptr;
    }
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const uint32_t resourceType = resourceRef.GetResourceType();
    if (resourceType == RESOURCE_NONE || !ValidateAccessPermission(&resourceRef))
    {
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TILE)
    {
        auto tileInstancePool = worldContext->GetTileInstancePool();
        if (tileInstancePool == nullptr)
        {
            return nullptr;
        }
        auto tileResource = FindTileRaw(&resourceRef);
        if (tileResource != nullptr)
        {
            result = std::make_unique<TileItem>(
                tileInstancePool->CreateTile(appContext_, tileResource, resourceRef.GetFingerprint()), resourceRef);
        }
    }
    if (resourceType == RESOURCE_COMPOSABLE_ITEM)
    {
        auto composableItemResource = FindComposableItem(&resourceRef);
        if (composableItemResource != nullptr)
        {
            result = std::move(
                ComposableItem::FromItemResource(worldContext, composableItemResource, resourceRef));
        }
    }

    if (resourceType == RESOURCE_ABILITY_ITEM)
    {
        auto abilityItemResource = FindAbilityItem(&resourceRef);
        if (abilityItemResource != nullptr)
        {
            result = std::move(AbilityItem::FromItemResource(appContext_, abilityItemResource, resourceRef));
        }
    }
    if (resourceType == RESOURCE_MATERIAL_ITEM)
    {
        auto materialItemResource = FindMaterialItem(&resourceRef);
        if (materialItemResource != nullptr)
        {
            result = std::move(MaterialItem::FromItemResource(appContext_, materialItemResource, resourceRef));
        }
    }
    if (result == nullptr)
    {
        return nullptr;
    }
    result->ReadItemMessage(worldContext, itemMessage);
    return result;
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext* worldContext,
                                                                  const ItemMessageResource& itemMessageResource)
const
{
    if (worldContext == nullptr)
    {
        return nullptr;
    }
    auto itemMessage = ItemMessage();
    itemMessage.set_locked(itemMessageResource.locked);
    itemMessage.set_amount(itemMessageResource.amount);
    itemMessageResource.item.WriteResourceRefMessage(*itemMessage.mutable_itemresourceref());
    for (auto& abilityItemResource : itemMessageResource.abilityItemRef)
    {
        ItemMessage* abilityItem = itemMessage.add_abilityitemref();
        abilityItemResource.item.WriteResourceRefMessage(*abilityItem->mutable_itemresourceref());
        abilityItem->set_locked(abilityItemResource.locked);
        abilityItem->set_amount(abilityItemResource.amount);
    }
    return FindItem(worldContext, itemMessage);
}
