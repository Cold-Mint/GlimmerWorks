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

#include "../../log/LogCat.h"
#include "../../world/WorldContext.h"
#include "../../utils/Box2DUtils.h"
#include "../component/RigidBody2DComponent.h"
#include "box2d/box2d.h"
#include "../../Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/HotBarComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/RayCast2DComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/utils/RandomUtils.h"

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
    dropItemSFX_ = appContext->GetResourceLocator()->FindAudio(&ref);
    audioManager_ = appContext->GetAudioManager();
}

void glimmer::PlayerControlSystem::Update(const float delta)
{
    if (WorldContext::IsEmptyEntityId(playerEntityID_))
    {
        return;
    }
    const auto playerComponent = entityManager_->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr)
    {
        return;
    }
    if (playerComponent->isFlying)
    {
        auto* transform2DComponent = entityManager_->GetComponent<Transform2DComponent>(playerEntityID_);
        if (transform2DComponent == nullptr)
        {
            return;
        }
        WorldVector2D velocity = {};
        velocity.x = playerComponent->horizontalInput * delta * FLY_SPEED;
        velocity.y = playerComponent->verticalInput * delta * FLY_SPEED;
        transform2DComponent->SetPosition(transform2DComponent->GetPosition() + velocity);
    }
    else
    {
        const auto rigidBody2DComponent = entityManager_->GetComponent<RigidBody2DComponent>(playerEntityID_);
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
        float targetAccX = playerComponent->movementAcceleration * playerComponent->horizontalInput;
        if (!isGrounded)
        {
            targetAccX *= playerComponent->airControlFactor;
        }
        float horizontalForce = massData.mass * targetAccX;
        if (playerComponent->horizontalInput == 0)
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
        if (playerComponent->jump)
        {
            playerComponent->jumpBuffer = JUMP_BUFFER_FRAMES;
            playerComponent->jump = false;
        }
        if (playerComponent->jumpBuffer > 0 && isGrounded)
        {
            b2Vec2 jumpImpulse = {0, massData.mass * playerComponent->jumpForce};
            b2Body_ApplyLinearImpulseToCenter(bodyId, jumpImpulse, true);
            // Consumption buffer
            // 消耗缓冲
            playerComponent->jumpBuffer = 0;
        }
        //Decremental buffering per frame
        //每帧递减缓冲
        if (playerComponent->jumpBuffer > 0)
        {
            playerComponent->jumpBuffer--;
        }

        if (std::abs(currentVel.x) > playerComponent->maxSpeed)
        {
            //The horizontal speed has exceeded the maximum speed.
            //水平方向速度，超过了最大速度。
            if (currentVel.x > 0)
            {
                b2Body_SetLinearVelocity(bodyId, {playerComponent->maxSpeed, currentVel.y});
            }
            else
            {
                b2Body_SetLinearVelocity(bodyId, {-playerComponent->maxSpeed, currentVel.y});
            }
        }

        float gravityForce = massData.mass * GRAVITY_SCALE;
        b2Body_ApplyForceToCenter(bodyId, {0, -gravityForce}, true);
    }
    auto hotBarComponent = entityShortCut_->GetHotBarComponent();
    auto itemContainerComponent = entityShortCut_->GetItemContainerComponent();
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    playerComponent->dropTimer += delta;
    if (playerComponent->dropPressed && playerComponent->dropTimer >= DROP_INTERVAL)
    {
        playerComponent->dropTimer -= DROP_INTERVAL;
        DropItem(itemContainer, hotBarComponent->GetSelectedSlot());
        playerComponent->dropPressed = false;
    }

    if (playerComponent->mouseLeftDown && playerComponent->item != nullptr)
    {
        slipTimer_ += delta;
        UseItem(playerComponent, itemContainer, hotBarComponent->GetSelectedSlot());
    }
    else
    {
        slipTimer_ = 0.0F;
    }
}

glimmer::GameSystemType glimmer::PlayerControlSystem::GetGameSystemType() const
{
    return GameSystemType::PlayerControlSystem;
}

bool glimmer::PlayerControlSystem::OnGround(const PlayerComponent* playerControlComponent) const
{
    if (playerControlComponent->groundCheckRayEntityIds.empty())
    {
        return false;
    }
    for (uint32_t rayCast2dItem : playerControlComponent->groundCheckRayEntityIds)
    {
        auto rayCast2DComponent = entityManager_->GetComponent<RayCast2DComponent>(rayCast2dItem);
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
    if (itemContainer == nullptr)
    {
        return;
    }
    auto item = itemContainer->TakeItem(index, 1);
    if (item == nullptr)
    {
        return;
    }
    audioManager_->TryPlayFree(AMBIENT, dropItemSFX_.get(), 0);
    const uint32_t droppedEntity = entityManager_->AddEntity();
    DroppedItemCreator droppedItemCreator{worldContext_};
    droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                              DroppedItemCreator::GetResourceRef());
    droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                              DroppedItemCreator::GetEntityItemMessage(
                                                  cameraTransform2DComponent_->
                                                  GetPosition(), std::move(item), 2));
}

void glimmer::PlayerControlSystem::UseItem(const PlayerComponent* playerComponent, ItemContainer* itemContainer,
                                           const uint8_t index)
{
    if (itemContainer == nullptr)
    {
        return;
    }
    std::unordered_set<std::string> popupAbility;
    const AbilityConfig* abilityConfig = playerComponent->item->GetAbilityConfig();
    if (abilityConfig != nullptr && slipTimer_ > 0.5F)
    {
        slipTimer_ = 0.0F;
        float fumbleChance = std::clamp(abilityConfig->fumbleProbability, 0.0F, 1.0F);
        if (fumbleChance > 0.0F)
        {
            auto randomValue = RandomUtils::Random<float>(0.0F, 1.0F);
            if (randomValue <= fumbleChance)
            {
                const uint32_t droppedEntity = entityManager_->AddEntity();
                DroppedItemCreator droppedItemCreator{worldContext_};
                droppedItemCreator.LoadTemplateComponents(droppedEntity,
                                                          DroppedItemCreator::GetResourceRef());
                droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                          DroppedItemCreator::GetEntityItemMessage(
                                                              cameraTransform2DComponent_
                                                              ->
                                                              GetPosition(),
                                                              std::move(itemContainer->TakeAllItem(
                                                                  index)),
                                                              2));
            }
        }
    }
    playerComponent->item->OnUse(worldContext_, playerEntityID_, abilityConfig, popupAbility);
}

void glimmer::PlayerControlSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType, uint32_t count)
{
    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr)
    {
        cameraComponent_ = entityShortCut_->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr)
    {
        cameraTransform2DComponent_ = entityShortCut_->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && playerEntityID_ == GAME_ENTITY_ID_INVALID)
    {
        playerEntityID_ = entityShortCut_->GetPlayer();
    }
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event& event)
{
    if (worldContext_ == nullptr)
    {
        return false;
    }
    if (WorldContext::IsEmptyEntityId(playerEntityID_))
    {
        return false;
    }
    auto* playerComponent = entityManager_->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr)
    {
        return false;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        playerComponent->mouseLeftDown = true;
        slipTimer_ = 0.0F;
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
    {
        playerComponent->mouseLeftDown = false;
    }
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        bool isAorD = false;
        if (event.key.key == SDLK_A)
        {
            playerComponent->pressedA = pressed;
            isAorD = true;
        }
        if (event.key.key == SDLK_D)
        {
            playerComponent->pressedD = pressed;
            isAorD = true;
        }
        if (isAorD)
        {
            playerComponent->horizontalInput = 0.0F;
            if (playerComponent->pressedA)
            {
                playerComponent->horizontalInput -= 1.0F;
            }
            if (playerComponent->pressedD)
            {
                playerComponent->horizontalInput += 1.0F;
            }
            const auto spiritRendererComponent = entityManager_->GetComponent<SpiritRendererComponent>(playerEntityID_);
            if (spiritRendererComponent != nullptr)
            {
                if (playerComponent->horizontalInput < -0.1F)
                {
                    playerComponent->facingLeft = true;
                    spiritRendererComponent->SetFlipH(true);
                }
                else if (playerComponent->horizontalInput > 0.1F)
                {
                    playerComponent->facingLeft = false;
                    spiritRendererComponent->SetFlipH(false);
                }
            }
        }

        bool isWorS = false;
        if (event.key.key == SDLK_W)
        {
            playerComponent->pressedW = pressed;
            isWorS = true;
        }
        if (event.key.key == SDLK_S)
        {
            playerComponent->pressedS = pressed;
            isWorS = true;
        }

        if (isWorS)
        {
            playerComponent->verticalInput = 0.0F;
            if (playerComponent->pressedW)
            {
                playerComponent->verticalInput += 1.0F;
            }
            if (playerComponent->pressedS)
            {
                playerComponent->verticalInput -= 1.0F;
            }
        }

        if (event.key.key == SDLK_W && pressed && !event.key.repeat)
        {
            playerComponent->jump = true;
        }
        if (event.key.key == SDLK_Q)
        {
            playerComponent->dropPressed = pressed;
            if (!pressed)
            {
                playerComponent->dropTimer = 0.0F;
            }
        }
    }
    return false;
}
