//
// Created by Cold-Mint on 2025/12/17.
//

#include "TileItem.h"

std::string glimmer::TileItem::GetId() const {
    return tile_.id;
}

std::string glimmer::TileItem::GetName() const {
    return tile_.name;
}

std::string glimmer::TileItem::GetDescription() const {
    return tile_.description;
}

void glimmer::TileItem::OnUse() {
}

std::shared_ptr<SDL_Texture> glimmer::TileItem::GetIcon() const {
    return tile_.texture;
}


void glimmer::TileItem::OnDrop() {
}
