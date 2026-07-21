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
#include <memory>
#include "core/log/LogCat.h"
#include "core/context/AppContext.h"
#include "ComposableItem.h"
#include "ItemAbilityFactory.h"
#include "core/mod/ResourceLocator.h"
#include "core/ecs/EcsTypes.h"


glimmer::ItemAbility* glimmer::AbilityItem::GetItemAbility() const
{
    return itemAbility_.get();
}

glimmer::AbilityItem::AbilityItem(const AbilityItemCreateParams& params) : id_(params.GetId()),
                                                                           name_(params.GetName()),
                                                                           description_(params.GetDescription()),
                                                                           iconResult_(params.GetIconResult()),
                                                                           itemAbility_(params.GetItemAbility()),
                                                                           maxDurability_(params.GetMaxDurability()),
                                                                           unbreakable_(params.IsUnbreakable()),
                                                                           canUseAlone_(params.IsCanUseAlone())
{
    SetTags(params.GetTags());
    SetResourceRef(params.GetResourceRef());
}

std::unique_ptr<glimmer::AbilityItem> glimmer::AbilityItem::FromItemResource(const AppContext* appContext,
                                                                             const AbilityItemResource* itemResource,
                                                                             const ResourceRef& resourceRef)
{
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->resourceId);
    if (const auto nameRes = appContext->GetResourceLocator()->FindString(&itemResource->name); nameRes != nullptr)
    {
        name = nameRes->value;
    }
    std::optional<std::string> description;
    if (auto descriptionRes = appContext->GetResourceLocator()->FindString(&itemResource->description); descriptionRes
        != nullptr)
    {
        description = descriptionRes->value;
    }

    const auto itemAbility =
        ItemAbilityFactory::CreateItemAbility(itemResource->ability, itemResource->abilityConfig);
    if (itemAbility == nullptr)
    {
        return nullptr;
    }

    AbilityItemCreateParams params;
    params.SetId(Resource::GenerateId(*itemResource));
    params.SetName(name);
    params.SetDescription(description);
    params.SetIconResult(appContext->GetResourceLocator()->FindTexture(&itemResource->texture));
    params.SetMaxDurability(itemResource->maxDurability);
    params.SetUnbreakable(itemResource->unbreakable);
    params.SetCanUseAlone(itemResource->canUseAlone);
    params.SetTags(itemResource->tags);
    params.SetResourceRef(resourceRef);
    return std::make_unique<AbilityItem>(params);
}

const glimmer::AbilityConfig* glimmer::AbilityItem::GetAbilityConfig() const
{
    if (itemAbility_ == nullptr)
    {
        return nullptr;
    }
    return itemAbility_->GetAbilityConfig();
}

void glimmer::AbilityItem::OnUse(WorldContext* worldContext, GameEntityID user, const AbilityConfig* abilityConfig,
                                 std::unordered_set<std::string, TransparentStringHash, std::equal_to<>>& popupAbility)
{
    if (canUseAlone_)
    {
        itemAbility_->OnUse(worldContext, user, abilityConfig, popupAbility);
    }
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
