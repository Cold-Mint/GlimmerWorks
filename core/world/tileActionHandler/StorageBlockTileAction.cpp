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
#include "StorageBlockTileAction.h"

#include "TargetResourceApplierContext.h"
#include "core/ecs/GameSystemType.h"
#include "core/mod/ResourceLocator.h"
#include "core/world/SystemScheduler.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"

void glimmer::StorageBlockTileAction::RollLootBoxSnapshot(const AppContext &appContext, const ResourceRef &lootTableRef,
                                                          LootBoxSnapshotMessage &snapshot) {
    if (!lootTableRef.IsValid()) {
        return;
    }
    const ResourceLocator *resourceLocator = appContext.GetResourceLocator();
    if (resourceLocator == nullptr) {
        return;
    }
    const LootResource *lootResource = resourceLocator->FindLoot(&lootTableRef);
    if (lootResource == nullptr) {
        return;
    }
    for (const std::vector<ItemMessage> lootItems = LootResource::GetLootItems(lootResource); const auto &itemMessage:
         lootItems) {
        snapshot.add_items()->CopyFrom(itemMessage);
    }
}

bool glimmer::StorageBlockTileAction::DoAction(const TargetResourceApplierContext &applierContext,
                                               const Tile &focusedTile,
                                               const TileStateMessage &focusedTileState) const {
    if (!focusedTile.IsStorageBlock()) {
        return false;
    }
    //Storage block: open its storage GUI instead of applying a tile action target.
    //存储方块：打开其存储GUI，而不是应用瓦片活动目标。
    const GameEntityID entityId = focusedTile.GetGameEntity(applierContext.GetActionOrigin());
    if (WorldContext::IsEmptyEntityId(entityId)) {
        return false;
    }
    const WorldContext *worldContext = applierContext.GetWorldContext();
    if (worldContext == nullptr) {
        return false;
    }
    EntityManager *entityManager = worldContext->GetEntityManager();
    if (entityManager == nullptr) {
        return false;
    }
    const auto *itemContainerComponent = entityManager->GetComponent<ItemContainerComponent>(
        entityId);
    if (itemContainerComponent == nullptr) {
        return false;
    }
    ItemContainer *container = itemContainerComponent->GetItemContainer();
    if (container == nullptr) {
        return false;
    }
    AppContext *appContext = worldContext->GetAppContext();
    if (appContext == nullptr) {
        return false;
    }
    //Loot chest: read the box's uuid so we can match against the player's snapshot.
    //战利品箱子：读出箱子 uuid，用于和玩家快照匹配。
    const ResourceRef &containerLootTable = focusedTile.GetContainerLootTable();
    std::string lootBoxUuid;
    if (containerLootTable.IsValid()) {
        lootBoxUuid = focusedTileState.uuid();
    }
    //First open by this player: roll and store a snapshot.
    //该玩家首次开箱：掷骰并存储快照。
    if (containerLootTable.IsValid() && !lootBoxUuid.empty()) {
        if (PlayerManifest *playerManifest = worldContext->GetPlayerManifest();
            playerManifest != nullptr &&
            !playerManifest->openedLootBoxSnapshot.contains(lootBoxUuid)) {
            LootBoxSnapshotMessage snapshot;
            RollLootBoxSnapshot(*appContext, containerLootTable, snapshot);
            playerManifest->openedLootBoxSnapshot[lootBoxUuid] =
                    std::move(snapshot);
        }
    }
    EntityShortCut *entityShortCut = worldContext->GetEntityShortCut();
    if (entityShortCut == nullptr) {
        return false;
    }
    entityShortCut->SetOpenedStorageContainer(container);
    entityShortCut->SetOpenedLootBoxUuid(lootBoxUuid);
    SystemScheduler *systemScheduler = worldContext->GetSystemScheduler();
    if (systemScheduler == nullptr) {
        return false;
    }
    LogCat::d(LogLabel::DEFAULT, "PushGuiSystemType(GameSystemType::StorageGUISystem)",
              "PushGuiSystemType(GameSystemType::StorageGUISystem)");
    systemScheduler->PushGuiSystemType(GameSystemType::StorageGUISystem);
    return true;
}
