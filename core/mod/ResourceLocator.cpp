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
    if (dataPackManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "dataPackManager_ == nullptr");
        return false;
    }
    return dataPackManager_->IsDependencySatisfied(
        resourceRef->GetSelfPackageId(), resourceRef->GetPackageId());
}

glimmer::ResourceLocator::ResourceLocator(AppContext* appContext) : appContext_(appContext)
{
    if (appContext_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "appContext_ == nullptr");
        return;
    }
    resourcePackManager_ = appContext_->GetResourcePackManager();
    if (resourcePackManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "resourcePackManager_ == nullptr");
        return;
    }
    const GraphicsContext* graphicsContext = appContext_->GetGraphicsContext();
    if (graphicsContext == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "graphicsContext == nullptr");
        return;
    }
    fixedColorManager_ =
        graphicsContext->GetFixedColorManager();
    if (fixedColorManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "fixedColorManager_ == nullptr");
        return;
    }
    lightMaskManager_ = graphicsContext->GetLightMaskManager();
    if (lightMaskManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "lightMaskManager_ == nullptr");
        return;
    }
    lightSourceManager_ = graphicsContext->GetLightSourceManager();
    if (lightSourceManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "lightSourceManager_ == nullptr");
        return;
    }
    const ModContext* modContext = appContext_->GetModContext();
    if (modContext == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "modContext == nullptr");
        return;
    }
    lootTableManager_ = modContext->GetLootTableManager();
    if (lootTableManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "lootTableManager_ == nullptr");
        return;
    }
    itemManager_ = modContext->GetItemManager();
    if (itemManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "itemManager_ == nullptr");
        return;
    }
    mobManager_ = modContext->GetMobManager();
    if (mobManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "mobManager_ == nullptr");
        return;
    }
    tileResourceManager_ = modContext->GetTileResourceManager();
    if (tileResourceManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "tileResourceManager_ == nullptr");
        return;
    }
    shapeManager_ = modContext->GetShapeManager();
    if (shapeManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "shapeManager_ == nullptr");
        return;
    }
    stringManager_ = modContext->GetStringManager();
    if (stringManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "stringManager_ == nullptr");
        return;
    }
    biomeDecoratorResourcesManager_ = modContext->GetBiomeDecoratorResourcesManager();
    if (biomeDecoratorResourcesManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "biomeDecoratorResourcesManager_ == nullptr");
        return;
    }
    dataPackManager_ = modContext->GetDataPackManager();
    if (dataPackManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "dataPackManager_ == nullptr");
        return;
    }
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourceLocator::FindTexture(
    const ResourceRef* resourceRef) const
{
    std::shared_ptr<TextureResourceResult> result = FindTextureRaw(resourceRef);
    if (result == nullptr)
    {
        if (resourcePackManager_ == nullptr)
        {
            LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourcePackManager == nullptr");
            return nullptr;
        }
        return resourcePackManager_->errorTexture_;
    }
    return result;
}

std::shared_ptr<glimmer::TextureResourceResult> glimmer::ResourceLocator::FindTextureRaw(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TEXTURE)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef->GetResourceType() != RESOURCE_TEXTURE");
        return nullptr;
    }
    if (resourcePackManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourcePackManager == nullptr");
        return nullptr;
    }
    if (!ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return resourcePackManager_->accessDeniedTexture_;
    }
    return resourcePackManager_->LoadTextureFromFile(appContext_, resourceRef);
}

std::shared_ptr<glimmer::AudioResourceResult> glimmer::ResourceLocator::FindAudio(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_AUDIO || !
        ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (resourcePackManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourcePackManager == nullptr");
        return nullptr;
    }
    return resourcePackManager_->LoadAudioFromFile(appContext_, resourceRef);
}

std::unique_ptr<glimmer::Color> glimmer::ResourceLocator::FindColor(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    const uint32_t resourceType = resourceRef->GetResourceType();
    if (!ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }

    if (resourceType == RESOURCE_COLOR)
    {
        if (resourcePackManager_ == nullptr)
        {
            LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourcePackManager == nullptr");
            return nullptr;
        }
        const ColorResource* colorResource = resourcePackManager_->LoadColorResFromFile(
            appContext_, resourceRef);
        if (colorResource == nullptr)
        {
            LogCat::w(__FILE__, __LINE__, __FUNCTION__, "colorResource == nullptr");
            return nullptr;
        }
        return std::make_unique<Color>(colorResource->ToColor());
    }
    if (resourceType == RESOURCE_FIXED_COLOR)
    {
        const FixedColorResource* fixedColorResource = fixedColorManager_->FindFixedColorResource(
            resourceRef->GetPackageId(),
            resourceRef->GetResourceKey());
        if (fixedColorResource == nullptr)
        {
            LogCat::w(__FILE__, __LINE__, __FUNCTION__, "fixedColorResource == nullptr");
            return nullptr;
        }
        return std::make_unique<Color>(fixedColorResource->ToColor());
    }
    LogCat::w(__FILE__, __LINE__, __FUNCTION__, "return nullptr");
    return nullptr;
}


glimmer::IShapeResource* glimmer::ResourceLocator::FindShape(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");

        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_SHAPE || !ValidateAccessPermission(resourceRef))
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (shapeManager_ == nullptr)
    {
        LogCat::e(__FILE__, __LINE__, __FUNCTION__, "shapeManager_ == nullptr");
        return nullptr;
    }
    return shapeManager_->FindShape(resourceRef->GetPackageId(),
                                    resourceRef->GetResourceKey());
}

glimmer::IBiomeDecoratorResource* glimmer::ResourceLocator::FindBiomeDecorator(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_BIOME_DECORATOR || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (biomeDecoratorResourcesManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "biomeDecoratorResourcesManager_ == nullptr");
        return nullptr;
    }
    return biomeDecoratorResourcesManager_->FindBiomeDecorator(
        resourceRef->GetPackageId(), resourceRef->GetResourceKey());
}

glimmer::StringResource* glimmer::ResourceLocator::FindString(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_STRING || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (stringManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "stringManager_ == nullptr");
        return nullptr;
    }
    return stringManager_->Find(resourceRef->GetPackageId(),
                                resourceRef->GetResourceKey());
}

glimmer::LightSourceResource* glimmer::ResourceLocator::FindLightSource(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_SOURCE || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (lightSourceManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "lightSourceManager_ == nullptr");
        return nullptr;
    }
    return lightSourceManager_->FindLightSourceResource(
        resourceRef->GetPackageId(),
        resourceRef->GetResourceKey());
}

glimmer::LightMaskResource* glimmer::ResourceLocator::FindLightMask(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LIGHT_MASK || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (lightMaskManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "lightMaskManager_ == nullptr");
        return nullptr;
    }
    return lightMaskManager_->FindLightMaskResource(resourceRef->GetPackageId(),
                                                    resourceRef->GetResourceKey());
}

glimmer::TileResource* glimmer::ResourceLocator::FindTileFallback(const ResourceRef* resourceRef,
                                                                  TileLayerType tileLayer) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (tileResourceManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "tileResource == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef))
    {
        return tileResourceManager_->GenerateAccessDeniedPlaceHolder(
            resourceRef->GetPackageId(), resourceRef->GetResourceKey(), tileLayer);
    }
    return tileResourceManager_->FindTileFallback(resourceRef->GetPackageId(),
                                                  resourceRef->GetResourceKey(),
                                                  tileLayer);
}

glimmer::TileResource* glimmer::ResourceLocator::FindTileRaw(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_TILE || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (tileResourceManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "tileResource == nullptr");
        return nullptr;
    }
    return tileResourceManager_->FindTileRaw(resourceRef->GetPackageId(),
                                             resourceRef->GetResourceKey());
}

glimmer::MobResource* glimmer::ResourceLocator::FindMob(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MOB || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (mobManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "mobManager == nullptr");
        return nullptr;
    }
    return mobManager_->FindMobResource(resourceRef->GetPackageId(),
                                        resourceRef->GetResourceKey());
}

glimmer::ComposableItemResource* glimmer::ResourceLocator::FindComposableItem(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_COMPOSABLE_ITEM || !
        ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (itemManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "itemManager == nullptr");
        return nullptr;
    }
    return itemManager_->FindComposableItemResource(resourceRef->GetPackageId(),
                                                    resourceRef->GetResourceKey());
}

glimmer::AbilityItemResource* glimmer::ResourceLocator::FindAbilityItem(
    const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_ABILITY_ITEM || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (itemManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "itemManager == nullptr");
        return nullptr;
    }
    return itemManager_->FindAbilityItemResource(resourceRef->GetPackageId(),
                                                 resourceRef->GetResourceKey());
}

glimmer::MaterialItemResource* glimmer::ResourceLocator::FindMaterialItem(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "resourceRef == nullptr");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_MATERIAL_ITEM || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (itemManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "itemManager == nullptr");
        return nullptr;
    }
    return itemManager_->FindMaterialItemResource(resourceRef->GetPackageId(),
                                                  resourceRef->GetResourceKey());
}

glimmer::LootResource* glimmer::ResourceLocator::FindLoot(const ResourceRef* resourceRef) const
{
    if (resourceRef == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "Resource is null");
        return nullptr;
    }
    if (resourceRef->GetResourceType() != RESOURCE_LOOT_TABLE || !ValidateAccessPermission(resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    if (lootTableManager_ == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "lootTableManager_ is null");
        return nullptr;
    }
    return lootTableManager_->Find(resourceRef->GetPackageId(),
                                   resourceRef->GetResourceKey());
}

std::unique_ptr<glimmer::Item> glimmer::ResourceLocator::FindItem(WorldContext* worldContext,
                                                                  const ItemMessage& itemMessage) const
{
    if (worldContext == nullptr)
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "worldContext is null");
        return nullptr;
    }
    ResourceRef resourceRef;
    resourceRef.ReadResourceRefMessage(itemMessage.itemresourceref());
    const uint32_t resourceType = resourceRef.GetResourceType();
    if (resourceType == RESOURCE_NONE || !ValidateAccessPermission(&resourceRef))
    {
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "ValidateAccessPermission");
        return nullptr;
    }
    std::unique_ptr<Item> result = nullptr;
    if (resourceType == RESOURCE_TILE)
    {
        auto tileInstancePool = worldContext->GetTileInstancePool();
        if (tileInstancePool == nullptr)
        {
            LogCat::w(__FILE__, __LINE__, __FUNCTION__, "tileInstancePool == nullptr");
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
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "result == nullptr");
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
        LogCat::w(__FILE__, __LINE__, __FUNCTION__, "worldContext == nullptr");
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
