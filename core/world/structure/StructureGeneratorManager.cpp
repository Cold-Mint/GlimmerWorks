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
#include "StructureGeneratorManager.h"

#include "core/log/LogCat.h"

std::vector<glimmer::TileVector2D> glimmer::StructureGeneratorManager::GetChunkDependencyTerrain(
    const uint32_t maxChunksOccupiedByStructure, const TileVector2D &centerChunkPosition) {
    std::vector<TileVector2D> result;
    //Center 1 block, add the left side edge maxChunksOccupiedByStructure and the right side edge, to obtain the total edge length. The square of the edge length equals the total area.
    //居中1个区块，加上左侧的边maxChunksOccupiedByStructure，右侧的边，得到总的边长。边长*边长等于总面积。
    const uint32_t sizeLength = maxChunksOccupiedByStructure * 2 + 1;
    result.reserve(sizeLength * sizeLength);
    const int radius = static_cast<int>(maxChunksOccupiedByStructure);
    const int startX = centerChunkPosition.x - radius * CHUNK_SIZE;
    const int startY = centerChunkPosition.y - radius * CHUNK_SIZE;
    for (int y = 0; y < static_cast<int>(sizeLength); ++y) {
        for (int x = 0; x < static_cast<int>(sizeLength); ++x) {
            result.emplace_back(startX + x * CHUNK_SIZE, startY + y * CHUNK_SIZE);
        }
    }
    return result;
}

void glimmer::StructureGeneratorManager::RegisterStructureGenerator(
    std::unique_ptr<IStructureGenerator> structureGenerator) {
    const StructureGeneratorType type = structureGenerator->GetStructureGeneratorType();
    structureGeneratorMap_.emplace(type, std::move(structureGenerator));
}

void glimmer::StructureGeneratorManager::ResetMaxChunksOccupiedByStructure() {
    maxChunksOccupiedByStructure_ = 0;
}

void glimmer::StructureGeneratorManager::UpdateMaxChunksOccupied(const uint32_t maxChunks) {
    maxChunksOccupiedByStructure_ = std::max(maxChunksOccupiedByStructure_, maxChunks);
}

uint32_t glimmer::StructureGeneratorManager::GetMaxChunksOccupiedByStructure() const {
    return maxChunksOccupiedByStructure_;
}

std::unique_ptr<glimmer::StructureInfo> glimmer::StructureGeneratorManager::Generate(WorldContext *worldContext,
    const TileVector2D &structuralOrigin, IStructureResource *structureResource) {
    const auto type = static_cast<StructureGeneratorType>(structureResource->generatorId);
    const auto iterator = structureGeneratorMap_.find(type);
    if (iterator == structureGeneratorMap_.end()) {
        LogCat::w(LogLabel::TERRAIN, std::source_location::current(), "structure_generator_not_registered",
                  "Structure generator is not registered: type={}", std::to_underlying(type));
        return nullptr;
    }
    const std::unique_ptr<IStructureGenerator> &structureGenerator = iterator->second;
    if (structureGenerator == nullptr) {
        return nullptr;
    }
    return structureGenerator->Generate(worldContext, structuralOrigin, structureResource);
}


uint32_t glimmer::StructureGeneratorManager::GetMaxExtent(IStructureResource *structureResource) {
    const auto type = static_cast<StructureGeneratorType>(structureResource->generatorId);
    const auto iterator = structureGeneratorMap_.find(type);
    if (iterator == structureGeneratorMap_.end()) {
        return 0;
    }
    const std::unique_ptr<IStructureGenerator> &structureGenerator = iterator->second;
    if (structureGenerator == nullptr) {
        return 0;
    }
    return structureGenerator->GetMaxExtent(structureResource);
}
