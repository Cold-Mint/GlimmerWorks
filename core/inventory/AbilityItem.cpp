//
// Created by coldmint on 2025/12/28.
//

#include "AbilityItem.h"


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

std::optional<glimmer::ResourceRef> glimmer::AbilityItem::ActualToResourceRef() {
    return Resource::ParseFromId(id_, RESOURCE_TYPE_ABILITY_ITEM);
}


void glimmer::AbilityItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
    if (canUseAlone_) {
        itemAbility_->OnUse(appContext, worldContext, user);
    }
}

std::unique_ptr<glimmer::Item> glimmer::AbilityItem::Clone() const {
    return std::make_unique<AbilityItem>(id_, name_, description_, icon_, itemAbility_->Clone(), canUseAlone_);
}
