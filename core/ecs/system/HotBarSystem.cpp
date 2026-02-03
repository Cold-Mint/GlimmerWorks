//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarSystem.h"
#include "../component/HotBarComonent.h"
#include "../../world/WorldContext.h"
#include "../component/ItemSlotComponent.h"
#include "core/ecs/component/GuiTransform2DComponent.h"
#include "core/ecs/component/PlayerControlComponent.h"


glimmer::HotBarSystem::HotBarSystem(WorldContext *worldContext)
    : GameSystem(worldContext) {
    RequireComponent<PlayerControlComponent>();
    RequireComponent<ItemContainerComponent>();
    RequireComponent<HotBarComponent>();
    RequireComponent<GuiTransform2DComponent>();
}

void glimmer::HotBarSystem::Update(float delta) {
    const auto hotBarEntity = worldContext_->GetHotBarEntity();
    if (WorldContext::IsEmptyEntityId(hotBarEntity)) {
        return;
    }
    auto *hotbarComponent = worldContext_->GetComponent<HotBarComponent>(hotBarEntity);
    auto SlotEntityList = hotbarComponent->GetSlotEntities();
    int selectedSlot = hotbarComponent->GetSelectedSlot();
    for (int i = 0; i < SlotEntityList.size(); ++i) {
        const auto entityId = SlotEntityList[i];
        if (WorldContext::IsEmptyEntityId(entityId)) {
            continue;
        }
        auto *slotComp = worldContext_->GetComponent<ItemSlotComponent>(entityId);
        slotComp->SetSelected(i == selectedSlot);
    }
}


uint8_t glimmer::HotBarSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR;
}

std::string glimmer::HotBarSystem::GetName() {
    return "glimmer.HotBarSystem";
}
