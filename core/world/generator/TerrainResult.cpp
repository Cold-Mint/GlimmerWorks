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
#include "TerrainResult.h"

#include "core/math/ChunkRelativeVector2D.h"
#include "core/math/CoordinateTransformer.h"


TerrainTileResult &glimmer::TerrainResult::GetMutableTerrainTileResult(const TerrainRelativeVector2D &localPosition) {
    return terrainTileResult_[CoordinateTransformer::GetArrayIndex(localPosition.x, localPosition.y, TERRAIN_SHIFT)];
}

void glimmer::TerrainResult::SetPosition(const TerrainVertexVector2D &position) {
    position_ = position;
}

const glimmer::TerrainVertexVector2D &glimmer::TerrainResult::GetPosition() const {
    return position_;
}

const TerrainTileResult &glimmer::TerrainResult::QueryTerrain(const TerrainRelativeVector2D &localPosition) const {
    return terrainTileResult_[CoordinateTransformer::GetArrayIndex(localPosition.x, localPosition.y, TERRAIN_SHIFT)];
}

void glimmer::TerrainResult::PushStructureTile(const StructureTile &structureTile) {
    terrainTileResultStack_.push(structureTile);
}

void glimmer::TerrainResult::PopAllStructureTile() {
    while (!terrainTileResultStack_.empty()) {
        terrainTileResultStack_.pop();
    }
}

void glimmer::TerrainResult::CommitStructure(const std::unique_ptr<std::bitset<TERRAIN_AREA> > &totalTerrain) {
    while (!terrainTileResultStack_.empty()) {
        const StructureTile &structureTile = terrainTileResultStack_.top();
        const uint32_t tileIndex = structureTile.GetTileIndex();
        if (tileIndex >= TERRAIN_AREA) {
            terrainTileResultStack_.pop();
            continue;
        }
        TerrainTileResult &terrainTileResult = terrainTileResult_[tileIndex];
        terrainTileResult.SetTerrainType(TerrainResultType::STRUCTURE);
        terrainTileResult.SetStructure(structureTile.GetLayerType(), structureTile.GetStructureResource());
        //Set the target point to 0 (mark it as an unplaceable structure to prevent subsequent structures from placing again at this point)
        //将目标点设置为0（标记为不可放置结构，防止后续结构再次向此点放置）
        totalTerrain->reset(tileIndex);
        terrainTileResultStack_.pop();
    }
}
