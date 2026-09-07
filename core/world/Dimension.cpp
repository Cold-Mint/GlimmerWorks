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
#include "Dimension.h"

#include <cmath>

#include "ChunkManager.h"
#include "TerrainManager.h"
#include "generator/ChunkGenerator.h"
#include "generator/ChunkLoader.h"
#include "core/log/LogCat.h"
#include "core/mod/Resource.h"
#include "core/saves/Saves.h"
#include "WorldContext.h"
#include "src/saves/dimension_manifest.pb.h"

glimmer::Dimension::Dimension(WorldContext *worldContext, DimensionResource *dimensionResource)
    : worldContext_(worldContext), dimensionResource_(dimensionResource) {
    if (dimensionResource_ == nullptr) {
        return;
    }
    dimensionId_ = Resource::GenerateId(dimensionResource_->packId, dimensionResource_->resourceId);
    dimensionFolderName_ = dimensionResource_->packId + "_" + dimensionResource_->resourceId;
    timeFlowSpeed_ = dimensionResource_->timeFlowSpeed;
    initialTime_ = dimensionResource_->initialTime;
}

glimmer::Dimension::~Dimension() = default;

void glimmer::Dimension::Init() {
    LogCat::i("Initializing dimension: ", dimensionId_);
    chunkManager_ = std::make_unique<ChunkManager>(worldContext_, dimensionFolderName_);
    terrainManager_ = std::make_unique<TerrainManager>(worldContext_);
    chunkGenerator_ = std::make_unique<
        ChunkGenerator>(worldContext_, worldContext_->GetWorldSeed(), dimensionResource_);
    chunkLoader_ = std::make_unique<ChunkLoader>(worldContext_, worldContext_->GetSaves(), dimensionFolderName_);
    const Saves *saves = worldContext_->GetSaves();
    if (saves == nullptr) {
        LogCat::e(std::source_location::current(), "saves == nullptr");
        return;
    }
    auto manifestOptional = saves->ReadDimensionManifest(dimensionFolderName_);
    if (manifestOptional.has_value()) {
        dimensionManifestMessage_ = manifestOptional.value();
    }
}

void glimmer::Dimension::SaveTime() const {
    if (worldContext_ == nullptr) {
        return;
    }
    Saves *saves = worldContext_->GetSaves();
    if (saves == nullptr) {
        return;
    }
    (void) saves->WriteDimensionManifest(dimensionFolderName_, dimensionManifestMessage_);
}

const std::string &glimmer::Dimension::GetDimensionId() const {
    return dimensionId_;
}

const std::string &glimmer::Dimension::GetDimensionFolderName() const {
    return dimensionFolderName_;
}

glimmer::DimensionResource *glimmer::Dimension::GetDimensionResource() const {
    return dimensionResource_;
}

glimmer::ChunkManager *glimmer::Dimension::GetChunkManager() const {
    return chunkManager_.get();
}

glimmer::TerrainManager *glimmer::Dimension::GetTerrainManager() const {
    return terrainManager_.get();
}

glimmer::ChunkGenerator *glimmer::Dimension::GetChunkGenerator() const {
    return chunkGenerator_.get();
}

glimmer::ChunkLoader *glimmer::Dimension::GetChunkLoader() const {
    return chunkLoader_.get();
}

float glimmer::Dimension::GetTimeOfDay() const {
    return timeOfDay_;
}

uint64_t glimmer::Dimension::GetDimensionTick(uint64_t globalTick) const {
// dimensionManifestMessage_.
}

void glimmer::Dimension::SetTimeOfDay(const DayNormalizedTime time) {
    timeOfDay_ = std::fmod(time, 1.0F);
    if (timeOfDay_ < 0.0F) {
        timeOfDay_ += 1.0F;
    }
}

float glimmer::Dimension::GetTimeFlowSpeed() const {
    return timeFlowSpeed_;
}

float glimmer::Dimension::GetInitialTime() const {
    return initialTime_;
}
