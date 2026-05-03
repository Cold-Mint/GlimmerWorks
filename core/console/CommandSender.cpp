//
// Created by coldmint on 2026/5/3.
//

#include "CommandSender.h"

void glimmer::CommandSender::SetPosition(const WorldVector2D &position) {
    this->position_ = position;
}

WorldVector2D glimmer::CommandSender::GetPosition() const {
    return this->position_;
}
