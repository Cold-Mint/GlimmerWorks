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

void glimmer::AbilityItem::OnUse(AppContext *appContext, WorldContext *worldContext, GameEntity *user) {
}

void glimmer::AbilityItem::OnDrop() {
}
