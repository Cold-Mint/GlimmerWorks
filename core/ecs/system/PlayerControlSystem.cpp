//
// Created by Cold-Mint on 2025/10/29.
//

#include "PlayerControlSystem.h"
#include "../../log/LogCat.h"


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
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    {
        const bool isPressed = event.type == SDL_EVENT_KEY_DOWN;
        switch (event.key.key)
        {
        case SDLK_W:
            for (auto& entity : entities)
            {
                auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
                if (control && control->enableWASD)
                {
                    control->moveUp = isPressed;
                }
            }
            return true;
        case SDLK_S:
            for (auto& entity : entities)
            {
                auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
                if (control && control->enableWASD)
                {
                    control->moveDown = isPressed;
                }
            }
            return true;
        case SDLK_A:
            for (auto& entity : entities)
            {
                auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
                if (control && control->enableWASD)
                {
                    control->moveLeft = isPressed;
                }
            }
            return true;
        case SDLK_D:
            for (auto& entity : entities)
            {
                auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
                if (control && control->enableWASD)
                {
                    control->moveRight = isPressed;
                }
            }
            return true;
        default:
            break;
        }
    }
    return false;
}
