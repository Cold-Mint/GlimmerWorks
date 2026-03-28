//
// Created by coldmint on 2026/3/28.
//

#include "FlowingTextCreator.h"

#include "component/FloatingTextComponent.h"

glimmer::FlowingTextCreator::FlowingTextCreator(WorldContext *worldContext, const std::string &text,
                                                WorldVector2D position) : IEntityCreator(worldContext) {
    worldContext_ = worldContext;
    text_ = text;
    position_ = position;
}

void glimmer::FlowingTextCreator::LoadTemplateComponents(GameEntity::ID id) {
    if (worldContext_ == nullptr) {
        return;
    }
    auto transform2dComponent = worldContext_->AddComponent<Transform2DComponent>(id);
    if (transform2dComponent != nullptr) {
        transform2dComponent->SetPosition(position_);
    }
    auto floatingTextComponent = worldContext_->AddComponent<FloatingTextComponent>(id);
    if (floatingTextComponent != nullptr) {
        floatingTextComponent->SetText(text_);
    }
}
