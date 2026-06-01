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
#include "ComposableItem.h"

#include "AbilityItem.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/world/TileInstancePool.h"
#include "core/world/WorldContext.h"
#include <utility>
#include <vector>
#include "../../core/mod/ResourceLocator.h"
#include "ability/ItemAbility.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/math/BackwardAllocStrategy.h"
#include "core/math/BalanceAllocStrategy.h"
#include "core/math/ForwardAllocStrategy.h"
#include "core/math/RandomAllocStrategy.h"
#include "core/utils/StringUtils.h"

void glimmer::ComposableItem::SwapItem(size_t index, ItemContainer* otherContainer, size_t otherIndex) const
{
    itemContainer_->SwapItem(index, otherContainer, otherIndex);
}

void glimmer::ComposableItem::RefreshAttributes()
{
    const size_t max = itemContainer_->GetCapacity();
    totalAbilityConfig_.Reset();
    for (size_t index = 0; index < max; index++)
    {
        Item* item = itemContainer_->GetItem(index);
        if (item == nullptr)
        {
            continue;
        }
        auto* abilityItem = dynamic_cast<AbilityItem*>(item);
        if (abilityItem == nullptr)
        {
            continue;
        }
        const AbilityConfig* abilityConfig = item->GetAbilityConfig();
        if (abilityConfig == nullptr)
        {
            continue;
        }
        totalAbilityConfig_ += *abilityConfig;
    }
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::ReplaceItem(const size_t index,
                                                                    std::unique_ptr<Item> item) const
{
    return itemContainer_->ReplaceItem(index, std::move(item));
}

size_t glimmer::ComposableItem::RemoveItemAbility(const std::string& id, const size_t amount) const
{
    return itemContainer_->RemoveItem(id, amount);
}

std::unique_ptr<glimmer::ComposableItem> glimmer::ComposableItem::FromItemResource(WorldContext* worldContext,
    const ComposableItemResource* itemResource, const ResourceRef& resourceRef)
{
    if (worldContext == nullptr)
    {
        return nullptr;
    }
    AppContext* appContext = worldContext->GetAppContext();
    if (itemResource == nullptr)
    {
        return nullptr;
    }
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->resourceId);
    const auto nameRes = appContext->GetResourceLocator()->FindString(&itemResource->name);
    if (nameRes != nullptr)
    {
        name = nameRes->value;
    }
    std::optional<std::string> description;
    auto descriptionRes = appContext->GetResourceLocator()->FindString(&itemResource->description);
    if (descriptionRes != nullptr)
    {
        description = descriptionRes->value;
    }
    std::unordered_set<uint64_t> tags;
    for (auto& tag : itemResource->tags)
    {
        tags.emplace(StringUtils::StringToUint64(tag));
    }
    std::unique_ptr<ComposableItem> result = std::make_unique<ComposableItem>(
        Resource::GenerateId(*itemResource), name,
        description,
        appContext->GetResourceLocator()->
                    FindTexture(&itemResource->texture),
        itemResource->slotSize,
        itemResource->maxDurability,
        itemResource->isUnbreakable, tags, resourceRef);
    //If the capability is not specified within the resource reference, then the default capability will be loaded.
    //如果没有在资源引用内指定能力，那么加载默认能力。
    size_t defaultAbilitySize = itemResource->defaultAbilityList.size();
    if (defaultAbilitySize > 0)
    {
        for (int i = 0; i < defaultAbilitySize; i++)
        {
            auto itemObj = appContext->GetResourceLocator()->FindItem(worldContext,
                                                                      itemResource->defaultAbilityList[i]);
            if (itemObj != nullptr)
            {
                (void)result->ReplaceItem(static_cast<size_t>(i), std::move(itemObj));
            }
        }
    }
    return result;
}

const glimmer::AbilityConfig* glimmer::ComposableItem::GetAbilityConfig() const
{
    return &totalAbilityConfig_;
}

void glimmer::ComposableItem::OnUse(WorldContext* worldContext, GameEntity::ID user, const AbilityConfig* abilityConfig,
                                    std::unordered_set<std::string>& popupAbility)
{
    const size_t max = itemContainer_->GetCapacity();
    //The ability to pop up
    //需要弹出的能力
    for (size_t index = 0; index < max; index++)
    {
        Item* item = itemContainer_->GetItem(index);
        if (item == nullptr)
        {
            continue;
        }
        auto* abilityItem = dynamic_cast<AbilityItem*>(item);
        if (abilityItem == nullptr)
        {
            continue;
        }
        ItemAbility* itemAbility = abilityItem->GetItemAbility();
        if (itemAbility == nullptr)
        {
            continue;
        }

        if (popupAbility.contains(itemAbility->GetId()))
        {
            //Mutual exclusivity
            //互斥
            const GameEntity::ID droppedEntity = worldContext->CreateEntity();
            DroppedItemCreator droppedItemCreator{worldContext};
            droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
            droppedItemCreator.MergeEntityItemMessage(droppedEntity, DroppedItemCreator::GetEntityItemMessage(
                                                          worldContext->GetCameraTransform2D()->
                                                                        GetPosition(),
                                                          itemContainer_->TakeAllItem(index), 2));
            continue;
        }
        itemAbility->OnUse(worldContext, user, abilityConfig, popupAbility);
    }
}


glimmer::ItemContainer* glimmer::ComposableItem::GetItemContainer() const
{
    return itemContainer_.get();
}

std::unique_ptr<glimmer::Item> glimmer::ComposableItem::Clone() const
{
    auto composableItem = std::make_unique<ComposableItem>(*this);
    composableItem->AddCallback();
    return composableItem;
}

void glimmer::ComposableItem::AddCallback()
{
    callback_ = itemContainer_->AddOnContentChanged([this](size_t index, Item* item, ContainerChangeType changeType)
    {
        switch (changeType)
        {
        case ContainerChangeType::ADD:
            RefreshAttributes();
            break;
        case ContainerChangeType::REMOVE:
            RefreshAttributes();
            break;
        }
    });
}

glimmer::ComposableItem::ComposableItem(const std::string& id, const std::string& name,
                                        const std::optional<std::string>& description,
                                        const std::shared_ptr<SDL_Texture>& icon, size_t maxSize,
                                        uint32_t maxDurability, bool isUnbreakable, std::unordered_set<uint64_t> tags,
                                        const ResourceRef& resourceRef)
{
    id_ = id;
    name_ = name;
    description_ = description;
    icon_ = icon;
    maxDurability_ = maxDurability;
    isUnbreakable_ = isUnbreakable;
    resourceRef_ = resourceRef;
    itemContainer_ = std::make_shared<ItemContainer>();
    itemContainer_->Resize(maxSize);
    tags_.insert(tags.begin(), tags.end());
    SetAllocStrategyType(static_cast<AllocStrategyTypeMessage>(RandomUtils::Random(
        0, 3)));
    AddCallback();
}

void glimmer::ComposableItem::SetAllocStrategyType(AllocStrategyTypeMessage allocStrategyType)
{
    switch (allocStrategyType)
    {
    case ALLOC_STRATEGY_BACKWARD:
        allocStrategyPtr_ = std::make_unique<BackwardAllocStrategy<uint32_t>>();
        break;
    case ALLOC_STRATEGY_FORWARD:
        allocStrategyPtr_ = std::make_unique<ForwardAllocStrategy<uint32_t>>();
        break;
    case ALLOC_STRATEGY_BALANCE:
        allocStrategyPtr_ = std::make_unique<BalanceAllocStrategy<uint32_t>>();
        break;
    case ALLOC_STRATEGY_RANDOM:
        allocStrategyPtr_ = std::make_unique<RandomAllocStrategy<uint32_t>>();
        break;
    case AllocStrategyTypeMessage_INT_MIN_SENTINEL_DO_NOT_USE_:
        break;
    case AllocStrategyTypeMessage_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
}

AllocStrategyTypeMessage glimmer::ComposableItem::GetAllocStrategyType() const
{
    if (allocStrategyPtr_ == nullptr)
    {
        return ALLOC_STRATEGY_FORWARD;
    }
    return allocStrategyPtr_->GetStrategyType();
}


void glimmer::ComposableItem::ReadItemMessage(WorldContext* worldContext, const ItemMessage& itemMessage)
{
    Item::ReadItemMessage(worldContext, itemMessage);
    if (worldContext == nullptr)
    {
        return;
    }
    const AppContext* appContext = worldContext->GetAppContext();
    if (appContext == nullptr)
    {
        return;
    }
    //Filling ability.
    //填充能力。
    if (itemContainer_ != nullptr)
    {
        const ResourceLocator* resourceLocator = appContext->GetResourceLocator();
        auto abilityItemRefSize = itemMessage.abilityitemref_size();
        for (int i = 0; i < itemContainer_->GetCapacity(); i++)
        {
            if (i >= abilityItemRefSize)
            {
                break;
            }
            const ItemMessage& abilityItemMessage = itemMessage.abilityitemref(i);
            std::unique_ptr<Item> item = resourceLocator->FindItem(worldContext, abilityItemMessage);
            if (item != nullptr)
            {
                std::unique_ptr<Item> result = ReplaceItem(static_cast<size_t>(i), std::move(item));
            }
        }
    }
    SetAllocStrategyType(itemMessage.durabilitystrategy());
}


void glimmer::ComposableItem::WriteItemMessage(ItemMessage& itemMessage) const
{
    Item::WriteItemMessage(itemMessage);
    itemMessage.clear_abilityitemref();
    itemMessage.set_durabilitystrategy(allocStrategyPtr_->GetStrategyType());
    if (itemContainer_ != nullptr)
    {
        for (int i = 0; i < itemContainer_->GetCapacity(); i++)
        {
            ItemMessage* abilityItemMessage = itemMessage.add_abilityitemref();
            const Item* item = itemContainer_->GetItem(i);
            if (item == nullptr)
            {
                continue;
            }
            item->WriteItemMessage(*abilityItemMessage);
        }
    }
}

glimmer::ComposableItem::~ComposableItem()
{
    if (itemContainer_ != nullptr)
    {
        itemContainer_->RemoveOnContentChanged(callback_);
    }
}

uint32_t glimmer::ComposableItem::GetMaxDurability() const
{
    return maxDurability_;
}

bool glimmer::ComposableItem::IsUnbreakable() const
{
    return isUnbreakable_;
}

const std::string& glimmer::ComposableItem::GetId() const
{
    return id_;
}

const std::string& glimmer::ComposableItem::GetName() const
{
    return name_;
}

const std::optional<std::string>& glimmer::ComposableItem::GetDescription() const
{
    return description_;
}

SDL_Texture* glimmer::ComposableItem::GetIcon() const
{
    return icon_.get();
}

unsigned glimmer::ComposableItem::GetRemaining() const
{
    if (isUnbreakable_)
    {
        return 0;
    }
    return maxDurability_ - GetUsedDurability();
}

void glimmer::ComposableItem::Reduce(unsigned value)
{
    if (itemContainer_ != nullptr)
    {
        std::vector<IAllocatable*> itemsList;
        for (size_t index = 0; index < itemContainer_->GetCapacity(); index++)
        {
            Item* item = itemContainer_->GetItem(index);
            if (item == nullptr)
            {
                continue;
            }
            itemsList.emplace_back(item);
        }
        allocStrategyPtr_->Allocate(itemsList, value);
    }
    AddUsedDurability(value);
}
