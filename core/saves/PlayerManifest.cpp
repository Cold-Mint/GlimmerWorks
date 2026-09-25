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
#include "PlayerManifest.h"

#include "core/log/LogCat.h"


uint32_t glimmer::PlayerManifest::Visited(const ResourceRef &dimensionsResourceRef) const {
    if (dimensionsResourceRef.GetResourceType() != RESOURCE_DIMENSION) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "invalid_dimension_resource_type",
                  "Expected RESOURCE_DIMENSION, but got resource type = {}",
                  std::to_underlying(dimensionsResourceRef.GetResourceType()));
        return -1;
    }
    const auto dimensionsResourceRefFingerprint = dimensionsResourceRef.GetFingerprint();
    ResourceRef tempResourceRef;
    const uint32_t size = visitedDimensions_.size();
    for (int i = 0; i < size; ++i) {
        tempResourceRef.ReadResourceRefMessage(visitedDimensions_[i].dimension());
        if (tempResourceRef.GetFingerprint() == dimensionsResourceRefFingerprint) {
            return i;
        }
    }
    return -1;
}

void glimmer::PlayerManifest::SwitchDimension(const ResourceRef &dimensionsResourceRef) {
    if (dimensionsResourceRef.GetResourceType() != RESOURCE_DIMENSION) {
        LogCat::w(LogLabel::DEFAULT, std::source_location::current(), "invalid_dimension_resource_type",
                  "Expected RESOURCE_DIMENSION, but got resource type = {}",
                  std::to_underlying(dimensionsResourceRef.GetResourceType()));
        return;
    }
    const uint32_t index = Visited(dimensionsResourceRef);
    if (index == -1) {
        //If the target dimension has not been visited, add a visit record for the corresponding dimension.
        //没有访问过目标维度，添加对应维度的访问记录。
        PlayerDimensionMessage playerDimensionMessage;
        dimensionsResourceRef.WriteResourceRefMessage(*playerDimensionMessage.mutable_dimension());
        playerDimensionMessage.set_needautomaticspawn(true);
        visitedDimensions_.push_back(playerDimensionMessage);
        currentDimensionIndex_ = visitedDimensions_.size() - 1;
        return;
    }
    currentDimensionIndex_ = index;
}


const PlayerDimensionMessage *glimmer::PlayerManifest::GetCurrentDimension() const {
    if (const size_t size = visitedDimensions_.size(); currentDimensionIndex_ >= size) {
        return nullptr;
    }
    return &visitedDimensions_.at(currentDimensionIndex_);
}

void glimmer::PlayerManifest::FromMessage(const PlayerMessage &playerMessage) {
    lastPlayedTime = playerMessage.lastplayedtime();
    permissionLevel = playerMessage.permissionlevel();
    int visitedDimensionsSize = playerMessage.visiteddimensions_size();
    visitedDimensions_.clear();
    visitedDimensions_.reserve(visitedDimensionsSize);
    for (int i = 0; i < visitedDimensionsSize; ++i) {
        visitedDimensions_.push_back(playerMessage.visiteddimensions(i));
    }
    entityItemMessage = playerMessage.entity();
    currentDimensionIndex_ = playerMessage.currentdimensionindex();
    LogCat::d(LogLabel::DEFAULT, "player_manifest_from_message", "Player manifest parsed: visitedDimensions={}",
              visitedDimensionsSize);
}

void glimmer::PlayerManifest::ToMessage(PlayerMessage &playerMessage) const {
    playerMessage.set_lastplayedtime(lastPlayedTime);
    playerMessage.set_permissionlevel(permissionLevel);
    playerMessage.set_currentdimensionindex(currentDimensionIndex_);
    playerMessage.mutable_visiteddimensions()->Clear();
    size_t visitedDimensionsSize = visitedDimensions_.size();
    for (int i = 0; i < visitedDimensionsSize; ++i) {
        playerMessage.mutable_visiteddimensions()->Add()->CopyFrom(visitedDimensions_[i]);
    }
    playerMessage.mutable_entity()->CopyFrom(entityItemMessage);
    LogCat::d(LogLabel::DEFAULT, "player_manifest_to_message", "Player manifest serialized: visitedDimensions={}",
              visitedDimensionsSize);
}
