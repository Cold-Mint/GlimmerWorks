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
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/GameSystemType.h"
#include "core/ecs/component/ColliderComponent.h"
#include "core/config/Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/SpiritRendererComponent.h"
#include "core/inventory/ItemContainer.h"
#include "core/inventory/ItemUseContext.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/world/SystemScheduler.h"
#include "core/world/TargetResourceApplier.h"
#include "src/saves/tile_state.pb.h"

glimmer::PlayerControlSystem::PlayerControlSystem(WorldContext *worldContext) : GameSystem(worldContext) {
    WatchComponent(COMPONENT_PLAYER);
    WatchComponent(COMPONENT_CAMERA);
    WatchComponent(COMPONENT_TRANSFORM_2D);
    itemUseContext_.worldContext = worldContext;
    itemUseContext_.popupAbility = &popupAbility_;
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "app_context_is_null", "appContext == nullptr");
        return;
    }
    ResourceRef ref;
    ref.SetSelfPackageId(RESOURCE_REF_CORE);
    ref.SetResourceType(RESOURCE_AUDIO);
    ref.SetResourceKey("sfx/drop_item");
    const ResourceLocator *resourceLocator = appContext->GetResourceLocator();
    if (resourceLocator == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "resource_locator_is_null",
                  "resourceLocator == nullptr");
        return;
    }
    dropItemSFXResult_ = resourceLocator->FindAudio(&ref);
    if (dropItemSFXResult_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "drop_item_sfx_result_is_null",
                  "dropItemSFXResult_ == nullptr");
        return;
    }
    const AudioContext *audioContext = appContext->GetAudioContext();
    if (audioContext == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "audio_context_is_null",
                  "audioContext == nullptr");
        return;
    }
    audioManager_ = audioContext->GetAudioManager();
    if (audioManager_ == nullptr) {
        LogCat::e(LogLabel::DEFAULT, std::source_location::current(), "audio_manager_is_null",
                  "audioManager_ == nullptr");
        return;
    }
    Init();
}

void glimmer::PlayerControlSystem::CheckDropItem(PlayerInputHandler *playerInputHandler,
                                                 const ItemContainer *itemContainer) const {
    if (!playerInputHandler->IsDropPressed()) {
        return;
    }
    DropItem(itemContainer, itemContainer->GetSelectIndex());
    playerInputHandler->SetDropPressed(false);
}

void glimmer::PlayerControlSystem::HandleMouseButton(const SDL_Event &event, PlayerInputHandler *playerInputHandler) {
    const uint32_t type = event.type;
    if (type != SDL_EVENT_MOUSE_BUTTON_DOWN && type != SDL_EVENT_MOUSE_BUTTON_UP) {
        return;
    }
    if (event.button.button == SDL_BUTTON_LEFT) {
        playerInputHandler->SetMouseLeftDown(type == SDL_EVENT_MOUSE_BUTTON_DOWN);
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
        playerInputHandler->SetMouseRightDown(type == SDL_EVENT_MOUSE_BUTTON_DOWN);
    }
}

void glimmer::PlayerControlSystem::HandleHorizontalInput(const SDL_Event &event, PlayerComponent *playerComponent,
                                                         PlayerInputHandler *playerInputHandler, bool pressed) const {
    bool isAorD = false;
    if (event.key.key == SDLK_A) {
        playerInputHandler->SetPressedA(pressed);
        isAorD = true;
    }
    if (event.key.key == SDLK_D) {
        playerInputHandler->SetPressedD(pressed);
        isAorD = true;
    }
    if (!isAorD) {
        return;
    }
    playerInputHandler->ResetHorizontalInput();
    if (playerInputHandler->IsPressedA()) {
        playerInputHandler->RemoveHorizontalInput(1.0F);
    }
    if (playerInputHandler->IsPressedD()) {
        playerInputHandler->AddHorizontalInput(1.0F);
    }
    UpdatePlayerFacing(playerComponent, playerInputHandler);
}

void glimmer::PlayerControlSystem::HandleVerticalInput(const SDL_Event &event, PlayerInputHandler *playerInputHandler,
                                                       const bool pressed) {
    bool isWorS = false;
    if (event.key.key == SDLK_W) {
        playerInputHandler->SetPressedW(pressed);
        isWorS = true;
    }
    if (event.key.key == SDLK_S) {
        playerInputHandler->SetPressedS(pressed);
        isWorS = true;
    }
    if (!isWorS) {
        return;
    }
    playerInputHandler->ResetVerticalInput();
    if (playerInputHandler->IsPressedW()) {
        playerInputHandler->AddVerticalInput(1.0F);
    }
    if (playerInputHandler->IsPressedS()) {
        playerInputHandler->RemoveVerticalInput(1.0F);
    }
}


glimmer::GameSystemType glimmer::PlayerControlSystem::GetGameSystemType() const {
    return GameSystemType::PlayerControlSystem;
}

void glimmer::PlayerControlSystem::DropItem(const ItemContainer *itemContainer, const uint8_t index) const {
    WorldContext *worldContext = GetWorldContext();

    if (itemContainer == nullptr) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "player_control_item_container_is_null",
                  "DropItem: itemContainer is nullptr");
        return;
    }
    const auto item = itemContainer->GetItem(index);
    if (item == nullptr) {
        return;
    }
    const ItemLockModule *itemLockModule = item->GetLockModule();
    if (itemLockModule == nullptr) {
        return;
    }
    if (itemLockModule->IsLocked()) {
        return;
    }
    auto takeItem = itemContainer->TakeItem(index, 1);
    if (takeItem == nullptr) {
        return;
    }
    LogCat::i(LogLabel::DEFAULT, "player_dropped_item", "Player dropped item: slot={}", index);
    if (dropItemSFXResult_ != nullptr) {
        if (MIX_Audio *audio = dropItemSFXResult_->GetResource(); audio != nullptr) {
            audioManager_->TryPlayFree(AudioType::AMBIENT, audio, 0);
        }
    }
    DroppedItemCreator::SpawnDroppedItem(worldContext,
                                         cameraTransform2DComponent_->GetPosition(), std::move(takeItem),
                                         DROPPED_ITEM_PICKUP_COOLDOWN_TICKS);
}


void glimmer::PlayerControlSystem::UpdateFlying(const PlayerInputHandler *playerInputHandler,
                                                Transform2DComponent *transform2DComponent) {
    WorldVector2D velocity = {};
    velocity.x = playerInputHandler->GetHorizontalInput() * FLY_SPEED * FIXED_TIME_STEP;
    velocity.y = playerInputHandler->GetVerticalInput() * FLY_SPEED * FIXED_TIME_STEP;
    transform2DComponent->SetPosition(transform2DComponent->GetPosition() + velocity);
}

void glimmer::PlayerControlSystem::UpdateGroundedMovement(PlayerInputHandler *playerInputHandler,
                                                          const PlayerComponent *playerComponent,
                                                          ColliderComponent *collider) {
    WorldVector2D velocity = collider->GetVelocity();
    const bool isGrounded = collider->IsGrounded();

    // Horizontal: accelerate/decelerate the velocity toward the target speed. This is a simple "move-toward"
    // controller so the movement feel is fully under our control (no contact solver involved).
    // 水平：让速度向目标速度加速/减速。这是一个简单的“逼近”控制器，使手感完全由我们掌控（不依赖接触求解器）。
    const float throttle = playerInputHandler->GetHorizontalInput();
    const float targetSpeed = playerComponent->GetMaxSpeed() * throttle;
    const float airSteer = isGrounded ? 1.0F : playerComponent->GetAirControlFactor();
    const float acceleration = playerComponent->GetMovementAcceleration() * airSteer;
    float maxDeltaSpeed = acceleration * FIXED_TIME_STEP;
    if (targetSpeed == 0.0F) {
        maxDeltaSpeed *= 3.0F; // snappier stop when releasing input 松开输入时更快停下
    }
    const float deltaSpeed = targetSpeed - velocity.x;
    if (std::abs(deltaSpeed) <= maxDeltaSpeed) {
        velocity.x = targetSpeed;
    } else {
        velocity.x += deltaSpeed > 0.0F ? maxDeltaSpeed : -maxDeltaSpeed;
    }

    // Jump with buffering. Sets the vertical velocity directly; gravity + collision are handled by PhysicsSystem.
    // 带缓冲的跳跃。直接设置竖直速度；重力与碰撞由 PhysicsSystem 处理。
    if (playerInputHandler->IsJump()) {
        playerInputHandler->SetJumpBuffer(JUMP_BUFFER_FRAMES);
        playerInputHandler->SetJump(false);
    }
    if (playerInputHandler->GetJumpBuffer() > 0 && isGrounded) {
        velocity.y = playerComponent->GetJumpForce();
        playerInputHandler->SetJumpBuffer(0);
    }
    if (playerInputHandler->GetJumpBuffer() > 0) {
        playerInputHandler->SetJumpBuffer(playerInputHandler->GetJumpBuffer() - 1);
    }

    collider->SetVelocity(velocity);
}

void glimmer::PlayerControlSystem::OnWatchedComponentChanged(GameComponentTypeMessage gameComponentType,
                                                             uint32_t count) {
    EntityShortCut *entityShortCut = GetEntityShortCut();

    if (gameComponentType == COMPONENT_CAMERA && cameraComponent_ == nullptr) {
        cameraComponent_ = entityShortCut->GetCameraComponent();
    }
    if (gameComponentType == COMPONENT_TRANSFORM_2D && cameraTransform2DComponent_ == nullptr) {
        cameraTransform2DComponent_ = entityShortCut->GetCameraTransform2DComponent();
    }
    if (gameComponentType == COMPONENT_PLAYER && playerEntityID_ == GAME_ENTITY_ID_INVALID) {
        playerEntityID_ = entityShortCut->GetPlayer();
        itemUseContext_.user = playerEntityID_;
        LogCat::i(LogLabel::DEFAULT, "player_control_player_bound", "PlayerControlSystem player entity bound: id={}",
                  playerEntityID_);
    }
}

void glimmer::PlayerControlSystem::OnTick(uint64_t tick) {
    EntityManager *entityManager = GetEntityManager();
    const EntityShortCut *entityShortCut = GetEntityShortCut();
    if (WorldContext::IsEmptyEntityId(playerEntityID_)) {
        return;
    }
    const auto playerComponent = entityManager->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr) {
        return;
    }
    const PlayerCapabilityHandler *playerCapabilityHandler = playerComponent->GetCapabilityHandler();
    if (playerCapabilityHandler == nullptr) {
        return;
    }
    PlayerInputHandler *playerInputHandler = playerComponent->GetInputHandler();
    if (playerInputHandler == nullptr) {
        return;
    }
    if (playerCapabilityHandler->IsFlying()) {
        const auto transform2DComponent = entityManager->GetComponent<Transform2DComponent>(playerEntityID_);
        if (transform2DComponent == nullptr) {
            return;
        }
        UpdateFlying(playerInputHandler, transform2DComponent);
    } else {
        const auto collider = entityManager->GetComponent<ColliderComponent>(playerEntityID_);
        if (collider == nullptr || !collider->IsEnabled()) {
            return;
        }
        UpdateGroundedMovement(playerInputHandler, playerComponent, collider);
    }
    const auto itemContainerComponent = entityShortCut->GetItemContainerComponent();
    const ItemContainer *itemContainer = itemContainerComponent->GetItemContainer();
    CheckDropItem(playerInputHandler, itemContainer);
    //The status must be updated here. Do not put it inside playerInputHandler->IsMouseLeftDown() || playerInputHandler->IsMouseRightDown(), because this will cause the condition to always be true.
    //必须在这里更新状态。不要放到playerInputHandler->IsMouseLeftDown() || playerInputHandler->IsMouseRightDown()内，因为这样会导致入永远为true。
    itemUseContext_.SetMouseLeftDown(playerInputHandler->IsMouseLeftDown());
    itemUseContext_.SetMouseRightDown(playerInputHandler->IsMouseRightDown());
    if (playerInputHandler->IsMouseLeftDown() || playerInputHandler->IsMouseRightDown()) {
        auto tileLayerEntities = entityManager->GetEntityIDWithComponents({COMPONENT_TILE_LAYER});
        std::ranges::sort(tileLayerEntities);
        itemUseContext_.focusedTileLayerComponents.clear();
        itemUseContext_.focusedTiles.clear();
        itemUseContext_.focusedTileStateMessages.clear();
        const TileVector2D &focusPosition = GetWorldContext()->GetFocusPosition();
        itemUseContext_.focusPosition = focusPosition;
        for (const auto &gameEntity: tileLayerEntities) {
            const auto tileLayerComponent = entityManager->GetComponent<TileLayerComponent>(
                gameEntity);
            if (tileLayerComponent == nullptr) {
                continue;
            }
            itemUseContext_.focusedTileLayerComponents.push_back(tileLayerComponent);
            itemUseContext_.focusedTiles.push_back(
                tileLayerComponent->GetSelfLayerTile(focusPosition));
            itemUseContext_.focusedTileStateMessages.push_back(
                tileLayerComponent->GetSelfLayerTileState(focusPosition));
        }

        Item *item = playerComponent->GetItem();
        bool handle = false;
        if (item != nullptr) {
            itemUseContext_.item = item;
            itemUseContext_.popupAbility->clear();
            handle = item->OnUse(itemUseContext_);
        }
        if (Item *emptyHandAutoUseItem = playerComponent->GetEmptyHandAutoUseItem(); !handle &&
            emptyHandAutoUseItem != nullptr) {
            itemUseContext_.item = emptyHandAutoUseItem;
            itemUseContext_.popupAbility->clear();
            handle = emptyHandAutoUseItem->OnUse(itemUseContext_);
        }
        if (!handle) {
            HandleFocusedTileActions();
        }
    }
}

void glimmer::PlayerControlSystem::HandleFocusedTileActions() const {
    WorldContext *worldContext = GetWorldContext();
    if (worldContext == nullptr) {
        return;
    }
    const AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return;
    }
    MainThreadDispatcher *mainThreadDispatcher = appContext->GetMainThreadDispatcher();
    if (mainThreadDispatcher == nullptr) {
        return;
    }
    //For the mouse click that is not handled by the item, treat it as an event call for the corresponding tile.
    //对于物品未处理的鼠标点击，看作对方块的事件调用。
    for (size_t i = 0; i < itemUseContext_.focusedTiles.size(); i++) {
        const Tile *focusedTile = itemUseContext_.focusedTiles[i];
        if (focusedTile == nullptr) {
            continue;
        }
        const TileStateMessage *tileState = i < itemUseContext_.focusedTileStateMessages.size()
                                                ? itemUseContext_.focusedTileStateMessages[i]
                                                : nullptr;
        //Resolve the origin cell of a multi-tile tile: the clicked cell plus its stored offset
        //yields the tile's top-left cell, then shift down by (height - 1) to reach the bottom-left
        //origin that TargetResourceApplier expects.
        //解析多方块瓦片的原点格子：点击格 + 存储的偏移得到瓦片左上格，再向下偏移 (height - 1) 得到
        //TargetResourceApplier 期望的左下原点。
        TileVector2D actionOrigin = itemUseContext_.focusPosition;
        if (tileState != nullptr) {
            TileVector2D offset(0, 0);
            offset.ReadVector2DIMessage(tileState->offset());
            actionOrigin.x += offset.x;
            actionOrigin.y += offset.y - (static_cast<int>(tileState->height()) - 1);
        }
        if (itemUseContext_.IsFirstMouseLeftDown()) {
            //Trigger the secondary action.
            //触发次要活动。
            const ResourceRef &secondaryActionTarget = focusedTile->GetSecondaryActionTarget();
            if (!secondaryActionTarget.IsValid()) {
                continue;
            }
            ApplyTileActionTarget(mainThreadDispatcher, worldContext, focusedTile, actionOrigin,
                                  secondaryActionTarget,
                                  BreakSource::TileSecondaryAction, PLACE_SOURCE_TILE_SECONDARY_ACTION);
            PlayTileActionSFX(focusedTile, false);
        }
        if (itemUseContext_.IsFirstMouseRightDown()) {
            //Trigger the primary action.
            //触发主要活动。
            if (focusedTile->IsStorageBlock()) {
                //Storage block: open its storage GUI instead of applying a tile action target.
                //存储方块：打开其存储GUI，而不是应用瓦片活动目标。
                const GameEntityID entityId = focusedTile->GetGameEntity(actionOrigin);
                if (!WorldContext::IsEmptyEntityId(entityId)) {
                    EntityManager *entityManager = worldContext->GetEntityManager();
                    if (entityManager != nullptr) {
                        const auto *itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(
                            entityId);
                        if (itemContainerComponent != nullptr) {
                            ItemContainer *container = itemContainerComponent->GetItemContainer();
                            if (container != nullptr) {
                                mainThreadDispatcher->PostToNextMainFrame([worldContext, container] {
                                    EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
                                    if (entityShortCut == nullptr) {
                                        return;
                                    }
                                    entityShortCut->SetOpenedStorageContainer(container);
                                    SystemScheduler *systemScheduler = worldContext->GetSystemScheduler();
                                    if (systemScheduler != nullptr) {
                                        systemScheduler->PushGuiSystemType(GameSystemType::StorageGUISystem);
                                    }
                                });
                                continue;
                            }
                        }
                    }
                }
            }
            const ResourceRef &primaryActionTarget = focusedTile->GetPrimaryActionTarget();
            if (!primaryActionTarget.IsValid()) {
                continue;
            }
            ApplyTileActionTarget(mainThreadDispatcher, worldContext, focusedTile, actionOrigin, primaryActionTarget,
                                  BreakSource::TilePrimaryAction, PLACE_SOURCE_TILE_PRIMARY_ACTION);
            PlayTileActionSFX(focusedTile, true);
        }
    }
}

void glimmer::PlayerControlSystem::ApplyTileActionTarget(MainThreadDispatcher *mainThreadDispatcher,
                                                         WorldContext *worldContext, const Tile *focusedTile,
                                                         const TileVector2D &actionOrigin,
                                                         const ResourceRef &actionTarget, BreakSource breakSource,
                                                         PlaceSourceMessage placeSource) const {
    mainThreadDispatcher->PostToNextMainFrame(
        [this, worldContext, focusedTile, actionTarget, breakSource, placeSource, actionOrigin] {
            TargetResourceApplier::Apply(worldContext, actionOrigin,
                                         focusedTile->GetLayerType(), actionTarget,
                                         breakSource, placeSource);
        });
}

void glimmer::PlayerControlSystem::PlayTileActionSFX(const Tile *tile, bool primary) const {
    if (tile == nullptr || audioManager_ == nullptr) {
        return;
    }
    const TileResourceData &tileResourceData = tile->GetResourceData();
    const AudioResourceResult *audioResourceResult = primary
                                                         ? tileResourceData.GetPrimaryActionSFX()
                                                         : tileResourceData.GetSecondaryActionSFX();
    if (audioResourceResult == nullptr) {
        return;
    }
    if (MIX_Audio *audio = audioResourceResult->GetResource(); audio != nullptr) {
        audioManager_->TryPlayFree(AudioType::AMBIENT, audio, 0);
    }
}

void glimmer::PlayerControlSystem::UpdatePlayerFacing(PlayerComponent *playerComponent,
                                                      const PlayerInputHandler *playerInputHandler) const {
    EntityManager *entityManager = GetEntityManager();
    const auto spiritRendererComponent = entityManager->GetComponent<SpiritRendererComponent>(playerEntityID_);
    if (spiritRendererComponent == nullptr) {
        return;
    }
    float horizontalInput = playerInputHandler->GetHorizontalInput();
    if (horizontalInput < -0.1F) {
        playerComponent->SetFacingLeft(true);
        spiritRendererComponent->SetFlipH(true);
    } else if (horizontalInput > 0.1F) {
        playerComponent->SetFacingLeft(false);
        spiritRendererComponent->SetFlipH(false);
    }
}

void glimmer::PlayerControlSystem::HandleKeyAction(const SDL_Event &event, PlayerInputHandler *playerInputHandler,
                                                   bool pressed) {
    if (event.key.key == SDLK_W && pressed && !event.key.repeat) {
        playerInputHandler->SetJump(true);
    }
    if (event.key.key == SDLK_Q) {
        playerInputHandler->SetDropPressed(pressed);
    }
}

bool glimmer::PlayerControlSystem::HandleEvent(const SDL_Event &event) {
    EntityManager *entityManager = GetEntityManager();
    if (entityManager == nullptr) {
        return false;
    }
    if (WorldContext::IsEmptyEntityId(playerEntityID_)) {
        return false;
    }
    auto playerComponent = entityManager->GetComponent<PlayerComponent>(playerEntityID_);
    if (playerComponent == nullptr) {
        return false;
    }
    PlayerInputHandler *playerInputHandler = playerComponent->GetInputHandler();
    if (playerInputHandler == nullptr) {
        return false;
    }
    HandleMouseButton(event, playerInputHandler);
    if (event.type != SDL_EVENT_KEY_DOWN && event.type != SDL_EVENT_KEY_UP) {
        return false;
    }
    bool pressed = event.type == SDL_EVENT_KEY_DOWN;
    HandleHorizontalInput(event, playerComponent, playerInputHandler, pressed);
    HandleVerticalInput(event, playerInputHandler, pressed);
    HandleKeyAction(event, playerInputHandler, pressed);
    return false;
}
