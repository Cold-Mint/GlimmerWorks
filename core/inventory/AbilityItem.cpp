//
// Created by coldmint on 2025/12/28.
//

#include "AbilityItem.h"
#include <utility>
#include "../log/LogCat.h"
#include "ComposableItem.h"
#include "ItemAbilityFactory.h"
#include "../mod/ResourceLocator.h"

std::string glimmer::AbilityItem::GetId() const {
    return id_;
}

std::string glimmer::AbilityItem::GetName() const {
    return name_;
}

std::string glimmer::AbilityItem::GetDescription() const {
    return description_;
}

std::shared_ptr<SDL_Texture> glimmer::AbilityItem::GetIcon() const {
    return icon_;
}

glimmer::ItemAbility *glimmer::AbilityItem::GetItemAbility() const {
    return itemAbility_.get();
}

std::unique_ptr<glimmer::AbilityItem> glimmer::AbilityItem::FromItemResource(AppContext *appContext,
                                                                             const AbilityItemResource *itemResource,
                                                                             const ResourceRef &resourceRef) {
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->key);
    const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
    if (nameRes.has_value()) {
        name = nameRes.value()->value;
    }
    std::string description = Resource::GenerateId(itemResource->packId, itemResource->key);;
    auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
    if (descriptionRes.has_value()) {
        description = descriptionRes.value()->value;
    }
    auto itemAbility =
            ItemAbilityFactory::CreateItemAbility(itemResource->ability, itemResource->abilityConfig);
    if (itemAbility == nullptr) {
        LogCat::e("An error occurred when constructing ability items, and the item ability is empty.");
        return nullptr;
    }
    return std::make_unique<AbilityItem>(Resource::GenerateId(*itemResource), name,
                                         description,
                                         appContext->GetResourceLocator()->FindTexture(
                                             itemResource->texture), std::move(itemAbility),
                                         itemResource->canUseAlone);
}

void glimmer::AbilityItem::OnUse(WorldContext *worldContext, const GameEntity::ID user) {
    if (canUseAlone_) {
        itemAbility_->OnUse(worldContext, user);
    }
}

std::optional<glimmer::ResourceRef> glimmer::AbilityItem::ActualToResourceRef() {
    return Resource::ParseFromId(id_, RESOURCE_TYPE_ABILITY_ITEM);
}

glimmer::AbilityItem::AbilityItem(std::string id, std::string name, std::string description,
                                  std::shared_ptr<SDL_Texture> icon, std::unique_ptr<ItemAbility> itemAbility,
                                  const bool canUseAlone) : id_(std::move(id)),
                                                            name_(std::move(name)),
                                                            description_(std::move(description)),
                                                            icon_(std::move(icon)),
                                                            itemAbility_(std::move(itemAbility)),
                                                            canUseAlone_(canUseAlone) {
}

std::unique_ptr<glimmer::Item> glimmer::AbilityItem::Clone() const {
    return std::make_unique<AbilityItem>(id_, name_, description_, icon_, itemAbility_->Clone(), canUseAlone_);
}
