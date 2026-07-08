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
#include "PlayerContext.h"

#include <utility>

#include "core/Constants.h"
#include "core/ecs/DroppedItemCreator.h"
#include "core/ecs/EntityManager.h"
#include "core/ecs/EntityShortCut.h"
#include "core/ecs/MobEntityCreator.h"
#include "core/ecs/component/DraggableComponent.h"
#include "core/ecs/component/HotBarComponent.h"
#include "core/ecs/component/ItemContainerComponent.h"
#include "core/ecs/component/ItemSlotComponent.h"
#include "core/ecs/component/PlayerComponent.h"
#include "core/ecs/component/Transform2DComponent.h"
#include "core/math/CoordinateTransformer.h"
#include "core/mod/ResourceLocator.h"
#include "core/mod/ResourceRef.h"
#include "core/mod/resourcePack/AudioResourceResult.h"
#include "core/scene/AppContext.h"
#include "core/world/WorldContext.h"

glimmer::PlayerContext::PlayerContext(WorldContext* worldContext) : worldContext_(worldContext)
{
    const AppContext* appContext = worldContext_->GetAppContext();
    if (appContext != nullptr)
    {
        ResourceRef ref;
        ref.SetSelfPackageId(RESOURCE_REF_CORE);
        ref.SetResourceType(RESOURCE_AUDIO);
        ref.SetResourceKey("sfx/item_break");
        itemBreakSFXResult_ = appContext->GetResourceLocator()->FindAudio(&ref);
        audioManager_ = appContext->GetAudioManager();
    }
}

glimmer::PlayerContext::~PlayerContext()
{
    if (worldContext_ == nullptr)
    {
        return;
    }
    const EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
    if (const EntityManager* entityManager = worldContext_->GetEntityManager(); entityShortCut == nullptr ||
        entityManager == nullptr)
    {
        return;
    }
    const ItemContainerComponent* itemContainerComponent = entityShortCut->GetItemContainerComponent();
    if (itemCallback_ == nullptr || itemContainerComponent == nullptr)
    {
        return;
    }
    if (ItemContainer* itemContainer = itemContainerComponent->GetItemContainer(); itemContainer != nullptr)
    {
        itemContainer->RemoveOnContentChanged(itemCallback_);
    }
}

void glimmer::PlayerContext::InitPlayer(const ResourceRef& resourceRef)
{
    EntityManager* entityManager = worldContext_->GetEntityManager();
    EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
    if (entityManager == nullptr || entityShortCut == nullptr)
    {
        return;
    }
    GameEntityID player = entityShortCut->GetPlayer();
    if (!WorldContext::IsEmptyEntityId(player))
    {
        return;
    }
    GameEntityID playerEntity = CreateOrLoadPlayer(resourceRef);
    if (!entityManager->HasComponent(playerEntity, COMPONENT_ITEM_CONTAINER))
    {
        InitPlayerInventory(playerEntity);
    }
    const auto* itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(playerEntity);
    if (itemContainerComponent == nullptr)
    {
        entityShortCut->SetPlayer(playerEntity);
        return;
    }
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        entityShortCut->SetPlayer(playerEntity);
        return;
    }
    auto* tempPlayerComponent = entityManager->GetComponent<PlayerComponent>(playerEntity);
    if (tempPlayerComponent != nullptr)
    {
        tempPlayerComponent->SetItem(itemContainer->GetItem(0));
    }
    itemCallback_ = itemContainer->AddOnContentChanged(
        [this, playerEntity](uint8_t index, Item* item, ContainerChangeType changeType)
        {
            OnPlayerItemChanged(index, item, changeType, playerEntity);
        });
    entityShortCut->SetPlayer(playerEntity);
}

uint32_t glimmer::PlayerContext::CreateOrLoadPlayer(const ResourceRef& resourceRef)
{
    uint32_t playerEntity = GAME_ENTITY_ID_INVALID;
    if (worldContext_->GetSaves()->PlayerExists())
    {
        auto playerMessage = worldContext_->GetSaves()->ReadPlayer();
        if (playerMessage.has_value())
        {
            playerEntity = worldContext_->GetChunkLoader()->RecoveryEntity(playerMessage->entity());
        }
    }
    if (!WorldContext::IsEmptyEntityId(playerEntity))
    {
        return playerEntity;
    }
    const auto firstTileTerrainY = worldContext_->GetChunkGenerator()->GetFirstTileTerrainY(0);
    playerEntity = worldContext_->GetEntityManager()->AddEntity();
    MobEntityCreator mobEntityCreator{worldContext_};
    mobEntityCreator.LoadTemplateComponents(playerEntity, resourceRef);
    mobEntityCreator.MergeEntityItemMessage(playerEntity,
                                            MobEntityCreator::GetEntityItemMessage(
                                                CoordinateTransformer::TileToWorld(
                                                    TileVector2D(0, firstTileTerrainY + 3))));
    return playerEntity;
}

void glimmer::PlayerContext::InitPlayerInventory(const uint32_t playerEntity)
{
    EntityManager* entityManager = worldContext_->GetEntityManager();
    const auto itemContainerComponent = entityManager->AddComponent<ItemContainerComponent>(playerEntity);
    if (itemContainerComponent == nullptr)
    {
        return;
    }
    ItemContainer* itemContainer = itemContainerComponent->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return;
    }
    itemContainer->Resize(HOT_BAR_SIZE * INVENTORY_ROW_COUNT);
    const auto& allInitialInventory = worldContext_->GetAppContext()->GetInitialInventoryManager()->
                                                     GetAllInitialInventory();
    const auto* playerTransform = entityManager->GetComponent<Transform2DComponent>(playerEntity);
    const WorldVector2D playerPos = playerTransform != nullptr ? playerTransform->GetPosition() : WorldVector2D();
    for (const auto& initialInventory : allInitialInventory)
    {
        for (const auto& addItem : initialInventory->addItems)
        {
            auto item = worldContext_->GetAppContext()->GetResourceLocator()->FindItem(worldContext_, addItem);
            if (item == nullptr)
            {
                continue;
            }
            std::unique_ptr<Item> returnItem = itemContainer->AddItem(std::move(item));
            if (returnItem == nullptr)
            {
                continue;
            }
            const uint32_t droppedEntity = entityManager->AddEntity();
            DroppedItemCreator droppedItemCreator{worldContext_};
            droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
            droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                      DroppedItemCreator::GetEntityItemMessage(
                                                          playerPos, std::move(returnItem), 2));
        }
    }
}

void glimmer::PlayerContext::OnPlayerItemChanged(const uint8_t index, Item* item,
                                                 const ContainerChangeType /*changeType*/,
                                                 const uint32_t playerEntity)
{
    const auto* hotBarComponent = worldContext_->GetEntityShortCut()->GetHotBarComponent();
    if (hotBarComponent == nullptr)
    {
        return;
    }
    if (index != hotBarComponent->GetSelectedSlot())
    {
        return;
    }
    auto* playerComponent = worldContext_->GetEntityManager()->GetComponent<PlayerComponent>(playerEntity);
    if (playerComponent == nullptr)
    {
        return;
    }
    if (item == nullptr)
    {
        playerComponent->SetItem(nullptr);
        return;
    }
    const ItemStackModule* itemStackModule = item->GetStackModule();
    if (itemStackModule == nullptr)
    {
        playerComponent->SetItem(nullptr);
        return;
    }
    if (const uint8_t amount = itemStackModule->GetAmount(); amount == 0)
    {
        playerComponent->SetItem(nullptr);
        return;
    }
    const ItemDurabilityModule* itemDurabilityModule = item->GetDurabilityModule();
    if (itemDurabilityModule == nullptr)
    {
        playerComponent->SetItem(nullptr);
        return;
    }
    if (!itemDurabilityModule->IsUnbreakable() && item->GetRemaining() == 0)
    {
        HandleItemBreak(item, playerEntity);
        return;
    }
    playerComponent->SetItem(item);
}

void glimmer::PlayerContext::HandleItemBreak(Item* item, const uint32_t playerEntity)
{
    if (itemBreakSFXResult_ != nullptr)
    {
        MIX_Audio* audio = itemBreakSFXResult_->GetResource();
        if (audio != nullptr)
        {
            audioManager_->TryPlayFree(AudioType::AMBIENT, audio, 0);
        }
    }
    auto* composableItem = dynamic_cast<ComposableItem*>(item);
    if (composableItem != nullptr)
    {
        DropComposableItemAbilities(composableItem);
    }
    auto* playerComponent = worldContext_->GetEntityManager()->GetComponent<PlayerComponent>(playerEntity);
    if (playerComponent != nullptr)
    {
        playerComponent->SetItem(nullptr);
    }
}

void glimmer::PlayerContext::DropComposableItemAbilities(ComposableItem* composableItem)
{
    ItemContainer* itemContainer = composableItem->GetItemContainer();
    if (itemContainer == nullptr)
    {
        return;
    }
    const uint8_t size = itemContainer->GetCapacity();
    const auto* cameraTransform = worldContext_->GetEntityShortCut()->GetCameraTransform2DComponent();
    const WorldVector2D dropPos = cameraTransform != nullptr ? cameraTransform->GetPosition() : WorldVector2D();
    for (uint8_t i = 0; i < size; i++)
    {
        Item* abilityItem = itemContainer->GetItem(i);
        if (abilityItem == nullptr)
        {
            continue;
        }
        const ItemStackModule* itemStackModule = abilityItem->GetStackModule();
        if (itemStackModule == nullptr)
        {
            continue;
        }
        const ItemLockModule* itemLockModule = abilityItem->GetLockModule();
        if (itemLockModule == nullptr)
        {
            continue;
        }
        const uint8_t abilityRemaining = abilityItem->GetRemaining();
        if (abilityRemaining == 0 || itemLockModule->IsLocked())
        {
            continue;
        }
        std::unique_ptr<Item> takeItem = itemContainer->TakeItem(i, itemStackModule->GetAmount());
        const uint32_t droppedEntity = worldContext_->GetEntityManager()->AddEntity();
        DroppedItemCreator droppedItemCreator{worldContext_};
        droppedItemCreator.LoadTemplateComponents(droppedEntity, DroppedItemCreator::GetResourceRef());
        droppedItemCreator.MergeEntityItemMessage(droppedEntity,
                                                  DroppedItemCreator::GetEntityItemMessage(
                                                      dropPos, std::move(takeItem), 2));
    }
}

void glimmer::PlayerContext::InitHotbar(ItemContainer* itemContainer) const
{
    EntityManager* entityManager = worldContext_->GetEntityManager();
    EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut();
    if (entityManager == nullptr || entityShortCut == nullptr || itemContainer == nullptr)
    {
        return;
    }
    auto hotBarEntity = entityManager->AddEntity();
    if (auto hotBarComponent = entityManager->AddComponent<HotBarComponent>(hotBarEntity); hotBarComponent != nullptr)
    {
        entityShortCut->SetHotBarComponent(hotBarComponent);
    }
    for (int i = 0; i < HOT_BAR_SIZE; i++)
    {
        const auto itemSlotEntity = entityManager->AddEntity();
        auto itemSlotComponent = entityManager->AddComponent<ItemSlotComponent>(
            itemSlotEntity, ItemSlotType::HotBar, itemContainer, i);
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        itemSlotComponent->SetSize({ITEM_SLOT_SIZE, ITEM_SLOT_SIZE});
        itemSlotComponent->Hide();
    }
}

void glimmer::PlayerContext::InitInventory(ItemContainer* itemContainer) const
{
    EntityManager* entityManager = worldContext_->GetEntityManager();
    if (const EntityShortCut* entityShortCut = worldContext_->GetEntityShortCut(); entityManager == nullptr ||
        entityShortCut == nullptr || itemContainer == nullptr)
    {
        return;
    }
    for (int i = 0; i < itemContainer->GetCapacity(); i++)
    {
        const auto itemSlotEntity = entityManager->AddEntity();
        auto itemSlotComponent = entityManager->AddComponent<ItemSlotComponent>(
            itemSlotEntity, ItemSlotType::Inventory, itemContainer, i);
        if (itemSlotComponent == nullptr)
        {
            continue;
        }
        if (const DraggableComponent* draggableComponent = entityManager->AddComponent<
            DraggableComponent>(itemSlotEntity); draggableComponent == nullptr)
        {
            continue;
        }
        itemSlotComponent->SetSize({ITEM_SLOT_SIZE, ITEM_SLOT_SIZE});
        itemSlotComponent->Hide();
    }
}
