//
// Created by Cold-Mint on 2025/10/29.
//

#include "CameraSystem.h"
#include "../../world/WorldContext.h"


void glimmer::CameraSystem::Render(SDL_Renderer *renderer) {
    auto *camera = worldContext_->GetCameraComponent();
    if (camera == nullptr) {
        return;
    }
    int winW = 0;
    int winH = 0;
    SDL_GetWindowSize(appContext_->GetWindow(), &winW, &winH);
    camera->SetSize(Vector2D(winW, winH));
}

std::string glimmer::CameraSystem::GetName() {
    return "glimmer.CameraSystem";
}
