//
// Created by coldmint on 2025/12/29.
//

#include "DiggingSystem.h"

#include "../../world/WorldContext.h"
#include "../component/CameraComponent.h"
#include "../component/Transform2DComponent.h"

void glimmer::DiggingSystem::Update(float delta) {
    // auto diggings = worldContext_->GetEntitiesWithComponents<DiggingComponent>();
    // if (diggings.empty()) {
    //     return;
    // }
    // for (auto digging: diggings) {
    //     auto *diggingComponent = worldContext_->GetComponent<DiggingComponent>(digging->GetID());
    //     if (diggingComponent == nullptr) {
    //         continue;
    //     }
    //     if (!diggingComponent->IsEnable()) {
    //         continue;
    //     }
    //     float digSpeed = 1.0f; // 以后可以来自工具
    //     diggingComponent->SetProgress((digSpeed / dig.hardness) * deltaTime);
    //
    //     if (dig.progress >= 1.0f) {
    //         BreakTile(dig.tilePos);
    //         digComp->currentDig.reset();
    //     }
    // }
}

void glimmer::DiggingSystem::Render(SDL_Renderer *renderer) {
    if (!cacheTexture) {
        for (uint8_t i = 0; i < 10; i++) {
            std::shared_ptr<SDL_Texture> texture = appContext_->GetResourcePackManager()->LoadTextureFromFile(
                appContext_,
                "cracks/cracks_" + std::to_string(i) + ".png");
            if (texture == nullptr) {
                LogCat::e("Failed to load cracks texture: " + std::to_string(i));
                continue;
            }
            textureList.push_back(texture);
        }
        cacheTexture = true;
        return;
    }
    auto diggingComponent = worldContext_->GetDiggingComponent();
    if (!diggingComponent->IsEnable()) {
        return;
    }
    LogCat::w("DiggingSystem::Enable");
    auto cameraTransform2D = worldContext_->GetCameraTransform2D();
    auto cameraComponent = worldContext_->GetCameraComponent();

    if (!cameraComponent->IsPointInViewport(cameraTransform2D->GetPosition(), diggingComponent->GetPosition())) {
        LogCat::w("DiggingSystem::Render: Point is not in viewport");
        return;
    }
    CameraVector2D cameraVector2d = cameraComponent->GetViewPortPosition(
        cameraTransform2D->GetPosition(), diggingComponent->GetPosition());
    float size = TILE_SIZE * cameraComponent->GetZoom();

    const auto maxIndex = static_cast<float>(textureList.size() - 1);
    uint8_t crackIndex = static_cast<uint8_t>(std::min(diggingComponent->GetProgress() * maxIndex, maxIndex));
    SDL_FRect dstRect = {cameraVector2d.x, cameraVector2d.y, size, size};

    auto &crackTexture = textureList[crackIndex];
    LogCat::w("DiggingSystem::dstRect");
    if (crackTexture) {
        // 先画调试矩形（红色）
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderRect(renderer, &dstRect);
        SDL_RenderTexture(renderer, crackTexture.get(), nullptr, &dstRect);
    }
}

uint8_t glimmer::DiggingSystem::GetRenderOrder() {
    return RENDER_ORDER_DIGGING;
}

std::string glimmer::DiggingSystem::GetName() {
    return "glimmer.DiggingSystem";
}
