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
#include "MaterialItem.h"

#include "core/utils/StringUtils.h"

glimmer::MaterialItem::MaterialItem(std::string id, std::string name, std::optional<std::string> description,
                                    std::shared_ptr<TextureResourceResult> iconResult,
                                    const std::vector<ItemTagResource>& tags,
                                    const ResourceRef& resourceRef) : id_(std::move(id)),
                                                                      name_(std::move(name)),
                                                                      description_(std::move(description)),
                                                                      iconResult_(std::move(iconResult))
{
    SetTags(tags);
    SetResourceRef(resourceRef);
    SetMaxStack(ITEM_MAX_STACK);
}

std::unique_ptr<glimmer::MaterialItem> glimmer::MaterialItem::FromItemResource(const AppContext* appContext,
                                                                               const MaterialItemResource* itemResource,
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
    return std::make_unique<MaterialItem>(Resource::GenerateId(*itemResource), name,
                                          description,
                                          appContext->GetResourceLocator()->FindTexture(
                                              &itemResource->texture), itemResource->tags, resourceRef);
}

const std::string& glimmer::MaterialItem::GetId() const
{
    return id_;
}

const std::string& glimmer::MaterialItem::GetName() const
{
    return name_;
}

const std::optional<std::string>& glimmer::MaterialItem::GetDescription() const
{
    return description_;
}

const glimmer::AbilityConfig* glimmer::MaterialItem::GetAbilityConfig() const
{
    return nullptr;
}

void glimmer::MaterialItem::Reduce(unsigned value)
{
    // MaterialItem has no durability to reduce
}


SDL_Texture* glimmer::MaterialItem::GetIcon() const
{
    if (iconResult_ == nullptr)
    {
        return nullptr;
    }
    return iconResult_->GetResource();
}

void glimmer::MaterialItem::OnUse(WorldContext* worldContext, uint32_t user, const AbilityConfig* abilityConfig,
                                  std::unordered_set<std::string, TransparentStringHash, std::equal_to<>>& popupAbility)
{
    // MaterialItem has no special use behavior
}


std::unique_ptr<glimmer::Item> glimmer::MaterialItem::Clone() const
{
    return std::make_unique<MaterialItem>(*this);
}
