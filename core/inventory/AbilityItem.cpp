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
#include "AbilityItem.h"
#include <utility>
#include "core/log/LogCat.h"
#include "ComposableItem.h"
#include "ItemAbilityFactory.h"
#include "core/mod/ResourceLocator.h"
#include "core/ecs/EcsTypes.h"


glimmer::ItemAbility* glimmer::AbilityItem::GetItemAbility() const
{
    return itemAbility_.get();
}

std::unique_ptr<glimmer::AbilityItem> glimmer::AbilityItem::FromItemResource(const AppContext* appContext,
                                                                             const AbilityItemResource* itemResource,
                                                                             const ResourceRef& resourceRef)
{
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

    auto itemAbility =
        ItemAbilityFactory::CreateItemAbility(itemResource->ability, itemResource->abilityConfig);
    if (itemAbility == nullptr)
    {
        LogCat::e("An error occurred when constructing ability items, and the item ability is empty.");
        return nullptr;
    }
    return std::make_unique<AbilityItem>(Resource::GenerateId(*itemResource), name,
                                         description,
                                         appContext->GetResourceLocator()->FindTexture(
                                             &itemResource->texture), std::move(itemAbility),
                                         itemResource->maxDurability, itemResource->isUnbreakable,
                                         itemResource->canUseAlone, itemResource->tags, resourceRef);
}

uint32_t glimmer::AbilityItem::GetMaxDurability() const
{
    return maxDurability_;
}

void glimmer::AbilityItem::Reduce(const unsigned value)
{
    if (isUnbreakable_)
    {
        return;
    }
    AddUsedDurability(value);
}

bool glimmer::AbilityItem::IsUnbreakable() const
{
    return isUnbreakable_;
}

const glimmer::AbilityConfig* glimmer::AbilityItem::GetAbilityConfig() const
{
    return itemAbility_->GetAbilityConfig();
}

void glimmer::AbilityItem::OnUse(WorldContext* worldContext, GameEntityID user, const AbilityConfig* abilityConfig,
                                 std::unordered_set<std::string>& popupAbility)
{
    if (canUseAlone_)
    {
        itemAbility_->OnUse(worldContext, user, abilityConfig, popupAbility);
    }
}


glimmer::AbilityItem::AbilityItem(std::string id, std::string name, std::optional<std::string> description,
                                  std::shared_ptr<TextureResourceResult> iconResult,
                                  std::shared_ptr<ItemAbility> itemAbility,
                                  uint32_t maxDurability,
                                  bool isUnbreakable, bool canUseAlone, const std::vector<ItemTagResource>& tags,
                                  const ResourceRef& resourceRef) : id_(std::move(id)),
                                                                    name_(std::move(name)),
                                                                    description_(std::move(description)),
                                                                    iconResult_(std::move(iconResult)),
                                                                    itemAbility_(std::move(itemAbility)),
                                                                    canUseAlone_(canUseAlone),
                                                                    maxDurability_(maxDurability),
                                                                    isUnbreakable_(isUnbreakable)
{
    SetTags(tags);
    resourceRef_ = resourceRef;
}

const std::string& glimmer::AbilityItem::GetId() const
{
    return id_;
}

const std::string& glimmer::AbilityItem::GetName() const
{
    return name_;
}

const std::optional<std::string>& glimmer::AbilityItem::GetDescription() const
{
    return description_;
}

SDL_Texture* glimmer::AbilityItem::GetIcon() const
{
    if (iconResult_ == nullptr)
    {
        return nullptr;
    }
    return iconResult_->GetResource();
}

std::unique_ptr<glimmer::Item> glimmer::AbilityItem::Clone() const
{
    return std::make_unique<AbilityItem>(*this);
}
