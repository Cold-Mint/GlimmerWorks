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

void glimmer::TerrainResult::PopStructureTile() {
    if (!terrainTileResultStack_.empty()) {
        terrainTileResultStack_.pop();
    }
}

void glimmer::TerrainResult::CommitStructure() {
    while (!terrainTileResultStack_.empty()) {
        const StructureTile &structureTile = terrainTileResultStack_.top();
        const uint32_t tileIndex = structureTile.GetTileIndex();
        if (tileIndex >= TERRAIN_SIZE) {
            terrainTileResultStack_.pop();
            continue;
        }
        TerrainTileResult &terrainTileResult = terrainTileResult_[tileIndex];
        terrainTileResult.SetTerrainType(TerrainResultType::STRUCTURE);
        terrainTileResult.SetStructure(structureTile.GetLayerType(), structureTile.GetStructureResource());
        terrainTileResultStack_.pop();
    }
}
