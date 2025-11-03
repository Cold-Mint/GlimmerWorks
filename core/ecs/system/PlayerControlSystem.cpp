//
// Created by Cold-Mint on 2025/10/29.
//

#include "PlayerControlSystem.h"
#include "../../log/LogCat.h"
#include "../component/CameraComponent.h"


void glimmer::PlayerControlSystem::Update(const float delta)
{
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    for (auto& entity : entities)
    {
        if (auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID()))
        {
            if (auto position = worldContext_->GetComponent<WorldPositionComponent>(entity->GetID()))
            {
                auto positionVec = position->GetPosition();
                if (control->moveUp)
                {
                    positionVec.y -= delta * 50;
                }
                if (control->moveDown)
                {
                    positionVec.y += delta * 50;
                }
                if (control->moveLeft)
                {
                    positionVec.x -= delta * 50;
                }
                if (control->moveRight)
                {
                    positionVec.x += delta * 50;
                }
                position->SetPosition(positionVec);
            }
        }
    }
}

std::string glimmer::PlayerControlSystem::GetName()
{
    return "glimmer.PlayerControlSystem";
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event& event)
{
    auto camera = worldContext_->GetCameraComponent();
    if (event.type == SDL_EVENT_MOUSE_WHEEL && camera)
    {
        float zoom = camera->GetZoom();
        constexpr float zoomStep = 0.1F;
        if (event.wheel.y > 0)
            zoom += zoomStep;
        else if (event.wheel.y < 0)
            zoom -= zoomStep;

        camera->SetZoom(zoom);
        LogCat::i("Camera zoom set to ", zoom);
        return true;
    }
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    {
        const bool isPressed = event.type == SDL_EVENT_KEY_DOWN;
        switch (event.key.key)
        {
        case SDLK_W:
        case SDLK_S:
        case SDLK_A:
        case SDLK_D:
            for (auto& entity : entities)
            {
                auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
                if (!control || !control->enableWASD) continue;

                switch (event.key.key)
                {
                case SDLK_W: control->moveUp = isPressed;
                    break;
                case SDLK_S: control->moveDown = isPressed;
                    break;
                case SDLK_A: control->moveLeft = isPressed;
                    break;
                case SDLK_D: control->moveRight = isPressed;
                    break;
                }
            }
            return true;
        default:
            break;
        }
    }

    return false;
}
