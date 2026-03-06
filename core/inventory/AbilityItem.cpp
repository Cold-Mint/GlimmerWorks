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

std::optional<std::string> glimmer::AbilityItem::GetDescription() const {
    return description_;
}


std::shared_ptr<SDL_Texture> glimmer::AbilityItem::GetIcon() const {
    return icon_;
}

glimmer::ItemAbility *glimmer::AbilityItem::GetItemAbility() const {
    return itemAbility_.get();
}

std::unique_ptr<glimmer::AbilityItem> glimmer::AbilityItem::FromItemResource(const AppContext *appContext,
                                                                             const AbilityItemResource *itemResource,
                                                                             const ResourceRef &resourceRef) {
    std::string name = Resource::GenerateId(itemResource->packId, itemResource->resourceId);
    const auto nameRes = appContext->GetResourceLocator()->FindString(itemResource->name);
    if (nameRes != nullptr) {
        name = nameRes->value;
    }
    std::optional<std::string> description;
    auto descriptionRes = appContext->GetResourceLocator()->FindString(itemResource->description);
    if (descriptionRes != nullptr) {
        description = descriptionRes->value;
    }
    auto itemAbility =
            ItemAbilityFactory::CreateItemAbility(appContext, itemResource->ability, itemResource->abilityConfig);
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

const glimmer::VariableConfig &glimmer::AbilityItem::GetVariableConfig() const {
    return itemAbility_->GetVariableConfig();
}

void glimmer::AbilityItem::OnUse(WorldContext *worldContext, GameEntity::ID user, const VariableConfig &abilityData,
                                 std::unordered_set<std::string> &popupAbility) {
    if (canUseAlone_) {
        itemAbility_->OnUse(worldContext, user, abilityData, popupAbility);
    }
}

std::optional<glimmer::ResourceRef> glimmer::AbilityItem::ActualToResourceRef() {
    return Resource::ParseFromId(id_, RESOURCE_TYPE_ABILITY_ITEM);
}

glimmer::AbilityItem::AbilityItem(std::string id, std::string name, std::optional<std::string> description,
                                  std::shared_ptr<SDL_Texture> icon, std::shared_ptr<ItemAbility> itemAbility,
                                  const bool canUseAlone) : id_(std::move(id)),
                                                            name_(std::move(name)),
                                                            description_(std::move(description)),
                                                            icon_(std::move(icon)),
                                                            itemAbility_(std::move(itemAbility)),
                                                            canUseAlone_(canUseAlone) {
}

std::unique_ptr<glimmer::Item> glimmer::AbilityItem::Clone() const {
    return std::make_unique<AbilityItem>(*this);
}
