/*
 * Copyright (C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * 版权(C) 2025  Cold-Mint <cold_mint@qq.com>
 *
 * 本程序是自由软件：你可以遵照自由软件基金会出版的GNU Affero通用公共许可证条款来重新分发和修改它
 * 该许可证的第3版，或者（由你选择）任何后续版本。
 *
 * 本程序的发布目的是希望它能有用，但没有任何担保；甚至没有适销性或特定用途适用性的默示担保。
 * 有关详细细节，请参阅GNU Affero通用公共许可证。
 *
 * 你应该已经收到一份GNU Affero通用公共许可证的副本。如果没有，请查阅<https://www.gnu.org/licenses/>。
 */
#include "PlayerControlSystem.h"

#include <random>

#include "core/log/LogCat.h"
#include "core/world/WorldContext.h"
#include "core/utils/Box2DUtils.h"
#include "core/ecs/component/RigidBody2DComponent.h"
#include "box2d/box2d.h"
#include "core/Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/HotBarComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/mod/resourcePack/AudioResourceResult.h"

glimmer::PlayerControlSystem::PlayerControlSystem(WorldContext* worldContext) : GameSystem(worldContext)
{
    WatchComponent(COMPONENT_PLAYER);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    const AppContext* appContext = worldContext->GetAppContext();
    ResourceRef ref;
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceType(RESOURCE_AUDIO);
    ref.SetResourceKey("sfx/drop_item");
    dropItemSFXResult_ = appContext->GetResourceLocator()->FindAudio(&ref);
    audioManager_ = appContext->GetAudioManager();
    Init();
}

void glimmer::PlayerControlSystem::Update(const float delta)
{
    EntityManager* entityManager = GetEntityManager();
    EntityShortCut* entityShortCut = GetEntityShortCut();

    if (WorldContext::IsEmptyEntityId(playerEntityID_))
    {
        return;
    }
    const auto playerComponent = entityManager->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr)
    {
        return;
    }
    if (playerComponent->IsFlying())
    {
        auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(playerEntityID_);
        if (transform2DComponent == nullptr)
        {
            return;
        }
        WorldVector2D velocity = {};
        velocity.x = playerComponent->GetHorizontalInput() * delta * FLY_SPEED;
        velocity.y = playerComponent->GetVerticalInput() * delta * FLY_SPEED;
        transform2DComponent->SetPosition(transform2DComponent->GetPosition() + velocity);
    }
    else
    {
        const auto rigidBody2DComponent = entityManager->GetComponent<RigidBody2DComponent>(playerEntityID_);
        if (rigidBody2DComponent == nullptr)
        {
            return;
        }

        if (!rigidBody2DComponent->IsReady())
        {
            return;
        }

        const b2BodyId bodyId = rigidBody2DComponent->GetBodyId();
        bool isGrounded = OnGround(playerComponent);
        const b2Vec2 currentVel = b2Body_GetLinearVelocity(bodyId);
        b2MassData massData = b2Body_GetMassData(bodyId);
        float targetAccX = playerComponent->GetMovementAcceleration() * playerComponent->GetHorizontalInput();
        if (!isGrounded)
        {
            targetAccX *= playerComponent->GetAirControlFactor();
        }
        float horizontalForce = massData.mass * targetAccX;
        if (playerComponent->GetHorizontalInput() == 0)
        {
            const float brakeFactor = isGrounded ? 8.0F : 2.0F;
            horizontalForce = -currentVel.x * massData.mass * brakeFactor;

            if (fabs(currentVel.x) < 0.1F)
            {
                b2Body_SetLinearVelocity(bodyId, {0.0F, currentVel.y});
                horizontalForce = 0.0F;
            }
        }
        b2Body_ApplyForceToCenter(bodyId, {horizontalForce, 0.0F}, true);
        if (playerComponent->IsJump())
        {
            playerComponent->SetJumpBuffer(JUMP_BUFFER_FRAMES);
            playerComponent->SetJump(false);
        }
        if (playerComponent->GetJumpBuffer() > 0 && isGrounded)
        {
            b2Vec2 jumpImpulse = {0, massData.mass * playerComponent->GetJumpForce()};
            b2Body_ApplyLinearImpulseToCenter(bodyId, jumpImpulse, true);
            // Consumption buffer
            // 消耗缓冲
            playerComponent->SetJumpBuffer(0);
        }
        //Decremental buffering per frame
        //每帧递减缓冲
        if (playerComponent->GetJumpBuffer() > 0)
        {
            playerComponent->SetJumpBuffer(playerComponent->GetJumpBuffer() - 1);
        }

        if (std::abs(currentVel.x) > playerComponent->GetMaxSpeed())
        {
            //The horizontal speed has exceeded the maximum speed.
            //水平方向速度，超过了最大速度。
            if (currentVel.x > 0)
            {
                b2Body_SetLinearVelocity(bodyId, {playerComponent->GetMaxSpeed(), currentVel.y});
            }
            else
            {
                b2Body_SetLinearVelocity(bodyId, {-playerComponent->GetMaxSpeed(), currentVel.y});
            }
        }

        float gravityForce = massData.mass * GRAVITY_SCALE;
        b2Body_ApplyForceToCenter(bodyId, {0, -gravityForce}, true);
    }
    auto hotBarComponent = entityShortCut->GetHotBarComponent();
    auto itemContainerComponent = entityShortCut->GetItemContainerComponent();
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    playerComponent->AddDropTimer(delta);
    if (playerComponent->IsDropPressed() && playerComponent->GetDropTimer() >= DROP_INTERVAL)
    {
        playerComponent->RemoveDropTimer(DROP_INTERVAL);
        DropItem(itemContainer, hotBarComponent->GetSelectedSlot());
        playerComponent->SetDropPressed(false);
    }

    if (playerComponent->IsMouseLeftDown())
    {
        Item* item = playerComponent->GetItem();
        if (item == nullptr)
        {
            item = playerComponent->GetEmptyHandAutoUseItem();
        }
        if (item == nullptr)
        {
            return;
        }
        UseItem(item);
    }
}

glimmer::GameSystemType glimmer::PlayerControlSystem::GetGameSystemType() const
{
    return GameSystemType::PlayerControlSystem;
}

bool glimmer::PlayerControlSystem::OnGround(const PlayerComponent* playerControlComponent) const
{
    EntityManager* entityManager = GetEntityManager();

    const std::vector<GameEntityID>& groundCheckRayEntityIds = playerControlComponent->GetGroundCheckRayEntityIds();
    if (groundCheckRayEntityIds.empty())
    {
        return false;
    }
    for (const uint32_t rayCast2dItem : groundCheckRayEntityIds)
    {
        auto rayCast2DComponent = entityManager->GetComponent<RayCast2DComponent>(rayCast2dItem);
        if (rayCast2DComponent == nullptr)
        {
            continue;
        }
        if (rayCast2DComponent->IsHit())
        {
            return true;
        }
    }
    return false;
}

void glimmer::PlayerControlSystem::DropItem(const ItemContainer* itemContainer, const uint8_t index) const
{
    EntityManager* entityManager = GetEntityManager();
    WorldContext* worldContext = GetWorldContext();

    if (itemContainer == nullptr)
    {
        return;
    }
    const auto item = itemContainer->GetItem(index);
    if (item == nullptr)
    {
        return;
    }
    if (item->IsLocked())
    {
        return;
    }
    auto takeItem = itemContainer->TakeItem(index, 1);
    if (takeItem == nullptr)
    {
        return;
    }
    if (dropItemSFXResult_ != nullptr)
    {
        MIX_Audio* audio = dropItemSFXResult_->GetResource();
        if (audio != nullptr)
        {
            audioManager_->TryPlayFree(AMBIENT, audio, 0);
        }
    }
    const uint32_t droppedEntity = entityManager->AddEntity();
    DroppedItemCreator droppedItemCreator{worldContext};
    droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                              DroppedItemCreator::GetResourceRef());
    droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                              DroppedItemCreator::GetEntityItemMessage(
                                                  cameraTransform2DComponent_->
                                                  GetPosition(), std::move(takeItem), 2));
}

void glimmer::PlayerControlSystem::UseItem(Item* item)
{
    WorldContext* worldContext = GetWorldContext();

    if (item == nullptr)
    {
        return;
    }
    popupAbility_.clear();
    item->OnUse(worldContext, playerEntityID_, item->GetAbilityConfig(), popupAbility_);
}

void glimmer::PlayerControlSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    EntityShortCut* entityShortCut = GetEntityShortCut();

    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && playerEntityID_ == GAME_ENTITY_ID_INVALID)
    {
        playerEntityID_ = entityShortCut->GetPlayer();
    }
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event& event)
{
    WorldContext* worldContext = GetWorldContext();
    EntityManager* entityManager = GetEntityManager();

    if (worldContext == nullptr)
    {
        return false;
    }
    if (WorldContext::IsEmptyEntityId(playerEntityID_))
    {
        return false;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        playerComponent->SetMouseLeftDown(true);
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
    {
        playerComponent->SetMouseLeftDown(false);
    }
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        bool isAorD = false;
        if (event.key.key == SDLK_A)
        {
            playerComponent->SetPressedA(pressed);
            isAorD = true;
        }
        if (event.key.key == SDLK_D)
        {
            playerComponent->SetPressedD(pressed);
            isAorD = true;
        }
        if (isAorD)
        {
            playerComponent->ResetHorizontalInput();
            if (playerComponent->IsPressedA())
            {
                playerComponent->RemoveHorizontalInput(1.0F);
            }
            if (playerComponent->IsPressedD())
            {
                playerComponent->AddHorizontalInput(1.0F);
            }
            const auto spiritRendererComponent = entityManager->GetComponent<SpiritRendererComponent>(playerEntityID_);
            if (spiritRendererComponent != nullptr)
            {
                float horizontalInput = playerComponent->GetHorizontalInput();
                if (horizontalInput < -0.1F)
                {
                    playerComponent->SetFacingLeft(true);
                    spiritRendererComponent->SetFlipH(true);
                }
                else if (horizontalInput > 0.1F)
                {
                    playerComponent->SetFacingLeft(false);
                    spiritRendererComponent->SetFlipH(false);
                }
            }
        }

        bool isWorS = false;
        if (event.key.key == SDLK_W)
        {
            playerComponent->SetPressedW(pressed);
            isWorS = true;
        }
        if (event.key.key == SDLK_S)
        {
            playerComponent->SetPressedS(pressed);
            isWorS = true;
        }

        if (isWorS)
        {
            playerComponent->ResetVerticalInput();
            if (playerComponent->IsPressedW())
            {
                playerComponent->AddVerticalInput(1.0F);
            }
            if (playerComponent->IsPressedS())
            {
                playerComponent->RemoveVerticalInput(1.0F);
            }
        }

        if (event.key.key == SDLK_W && pressed && !event.key.repeat)
        {
            playerComponent->SetJump(true);
        }
        if (event.key.key == SDLK_Q)
        {
            playerComponent->SetDropPressed(pressed);
            if (!pressed)
            {
                playerComponent->ResetDropTimer();
            }
        }
    }
    return false;
}
