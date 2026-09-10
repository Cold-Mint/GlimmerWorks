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
#include "WorldSaver.h"

#include "core/log/LogCat.h"
#include "core/mod/ResourceRef.h"
#include "core/utils/TimeUtils.h"
#include "WorldContext.h"

glimmer::WorldSaver::WorldSaver(WorldContext *worldContext) : worldContext_(worldContext) {
}

void glimmer::WorldSaver::SaveEntity(EntityItemMessage *entityItemMessage, const GameEntityID entityId) const {
    LogCat::d("world_context_save_entity", "SaveEntity: entityId={}", entityId);
    entityItemMessage->mutable_gameentity()->set_id(entityId);
    const ResourceRef *resourceRef = worldContext_->GetEntityManager()->GetResourceRef(entityId);
    if (resourceRef != nullptr) {
        resourceRef->WriteResourceRefMessage(*entityItemMessage->mutable_resourceref());
    }
    std::vector<GameComponent *> components = worldContext_->GetEntityManager()->GetAllComponent(entityId);
    auto mutableComponents = entityItemMessage->mutable_components();
    for (auto &componentItem: components) {
        auto stringOptional = componentItem->Serialize();
        if (stringOptional.has_value()) {
            ComponentMessage *componentMessage = mutableComponents->Add();
            componentMessage->set_type(componentItem->GetComponentType());
            componentMessage->set_data(stringOptional.value());
        }
    }
    LogCat::d("world_context_save_entity_completed", "SaveEntity completed: entityId={}, components={}", entityId,
              components.size());
}

void glimmer::WorldSaver::SaveGame() {
    if (saving_) {
        LogCat::w(std::source_location::current(), "world_context_save_in_progress",
                  "Save already in progress, ignoring");
        return;
    }
    LogCat::i("world_context_save_starting", "Starting game save: {}", worldContext_->GetMapManifest()->name);
    saving_ = true;
    const Saves *saves = worldContext_->GetSaves();
    if (saves == nullptr) {
        LogCat::e(std::source_location::current(), "saves_is_null", "saves is nullptr");
        saving_ = false;
        return;
    }
    auto mapManifestMessageData = saves->ReadMapManifest();
    if (!mapManifestMessageData.has_value()) {
        LogCat::w(std::source_location::current(), "world_context_read_map_manifest_failed",
                  "Failed to read map manifest");
        saving_ = false;
        return;
    }
    const long endTime = TimeUtils::GetCurrentTimeMs();
    mapManifestMessageData->set_globaltickcount(worldContext_->GetGlobalTick());
    mapManifestMessageData->set_entityidindex(worldContext_->GetEntityManager()->GetEntityIndex());
    if (!saves->WriteMapManifest(mapManifestMessageData.value())) {
        LogCat::w(std::source_location::current(), "world_context_write_map_manifest_failed",
                  "Failed to write map manifest");
        saving_ = false;
        return;
    }
    auto player = worldContext_->GetEntityShortCut()->GetPlayer();
    if (!WorldContext::IsEmptyEntityId(player) && worldContext_->GetEntityManager()->IsPersistable(player)) {
        PlayerMessage playerMessage;
        playerMessage.set_lastplayedtime(endTime);

        SaveEntity(playerMessage.mutable_entity(), player);
        (void) saves->WriteLocalPlayer(playerMessage);
        LogCat::i("world_context_player_saved", "Player saved");
    } else {
        LogCat::d("world_context_player_save_skipped", "Player save skipped: isEmpty={}, persistable={}",
                  WorldContext::IsEmptyEntityId(player), worldContext_->GetEntityManager()->IsPersistable(player));
    }
    saving_ = false;
}
