//
// Created by Cold-Mint on 2025/10/29.
//

#include "CameraSystem.h"


void glimmer::CameraSystem::Render(SDL_Renderer* renderer)
{
    const auto camera = worldContext_->GetCameraComponent();
    if (camera == nullptr)
    {
        return;
    }
    int w, h;
    SDL_GetWindowSize(appContext_->GetWindow(), &w, &h);
    camera->SetSize(Vector2D(w, h));
}

std::string glimmer::CameraSystem::GetName()
{
    return "glimmer.CameraSystem";
}
