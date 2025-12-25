//
// Created by Cold-Mint on 2025/12/18.
//

#include "HotBarSystem.h"
#include "../component/HotBarComonent.h"
#include "../../world/WorldContext.h"
#include "../component/ItemSlotComponent.h"
#include "../component/Transform2DComponent.h"


void glimmer::HotBarSystem::Update(float delta) {
    const auto *hotBar = worldContext_->GetHotBarComponent();
    if (!hotBar) return;

    Vector2D pos = hotBar->GetPosition();
    const float slotSize = 40.0F * appContext_->GetConfig()->window.uiScale;
    float padding = 8.0F * appContext_->GetConfig()->window.uiScale;

    float startX = pos.x;
    float startY = pos.y;

    const auto &slots = hotBar->GetSlotEntities();
    int selectedSlot = hotBar->GetSelectedSlot();

    for (size_t i = 0; i < slots.size(); ++i) {
        auto *ent = slots[i];
        if (!ent) continue;

        auto *trans = worldContext_->GetComponent<Transform2DComponent>(ent->GetID());
        auto *slotComp = worldContext_->GetComponent<ItemSlotComponent>(ent->GetID());

        if (trans) {
            trans->SetPosition({startX + static_cast<float>(i) * (slotSize + padding), startY});
        }

        if (slotComp) {
            slotComp->SetSelected(static_cast<int>(i) == selectedSlot);
        }
    }
}


uint8_t glimmer::HotBarSystem::GetRenderOrder() {
    return RENDER_ORDER_DEBUG_HOTBAR;
}

std::string glimmer::HotBarSystem::GetName() {
    return "glimmer.HotBarSystem";
}
