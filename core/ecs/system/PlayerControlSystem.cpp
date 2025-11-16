//
// Created by Cold-Mint on 2025/10/29.
//

#include "PlayerControlSystem.h"
#include "../../log/LogCat.h"
#include "../../world/WorldContext.h"
#include "../../ecs/component/CameraComponent.h"
#include "../component/RigidBody2DComponent.h"


void glimmer::PlayerControlSystem::Update(const float delta)
{
    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent, RigidBody2DComponent>();
    for (auto& entity : entities)
    {
        const auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
        const auto rigid = worldContext_->GetComponent<RigidBody2DComponent>(entity->GetID());
        if (control == nullptr || rigid == nullptr || !rigid->IsReady())
        {
            continue;
        }

        const b2BodyId bodyId = rigid->GetBodyId();
        const b2Vec2 vel = b2Body_GetLinearVelocity(bodyId);
        float vx = 0.0F;
        if (control->moveLeft)
        {
            vx -= PLAYER_MOVE_SPEED;
        }
        if (control->moveRight)
        {
            vx += PLAYER_MOVE_SPEED;
        }
        float vy = vel.y;
        if (control->jump)
        {
            bool onGround = fabsf(vel.y) < 0.01f;
            if (onGround)
            {
                vy = 10.0F;
            }
            control->jump = false;
        }

        b2Body_SetLinearVelocity(bodyId, {vx, vy});
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
        zoom += (event.wheel.y > 0 ? zoomStep : -zoomStep);

        camera->SetZoom(zoom);
        LogCat::i("Camera zoom set to ", zoom);
        return true;
    }

    const auto entities = worldContext_->GetEntitiesWithComponents<PlayerControlComponent>();
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        for (auto& entity : entities)
        {
            auto control = worldContext_->GetComponent<PlayerControlComponent>(entity->GetID());
            if (!control) continue;
            switch (event.key.key)
            {
            case SDLK_A: control->moveLeft = pressed;
                return true;
            case SDLK_D: control->moveRight = pressed;
                return true;
            case SDLK_SPACE:
                if (pressed)
                {
                    control->jump = true;
                }
                return true;

            default:
                return false;
            }
        }
    }

    return false;
}
