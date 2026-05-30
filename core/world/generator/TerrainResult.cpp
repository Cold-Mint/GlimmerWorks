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

void glimmer::TerrainResult::SetTerrainTileResult(const int x, const int y, const TerrainTileResult &result) {
    const int idx = y * CHUNK_SIZE + x;
    terrainTileResult_[idx] = result;
}

void glimmer::TerrainResult::SetPosition(const TileVector2D &position) {
    position_ = position;
}

glimmer::TileVector2D glimmer::TerrainResult::GetPosition() const {
    return position_;
}

const TerrainTileResult &glimmer::TerrainResult::QueryTerrain(const int x, const int y) const {
    const int idx = y * CHUNK_SIZE + x;
    // up
    // 上
    if (y == CHUNK_SIZE && x >= 0 && x < CHUNK_SIZE) {
        return upTerrainTileResult_[x];
    }

    // down
    // 下
    if (y == -1 && x >= 0 && x < CHUNK_SIZE) {
        return downTerrainTileResult_[x];
    }

    // left
    // 左
    if (x == -1 && y >= 0 && y < CHUNK_SIZE) {
        return leftTerrainTileResult_[y];
    }

    // right
    // 右
    if (x == CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE) {
        return rightTerrainTileResult_[y];
    }
    // within the chunk
    // 在 chunk 内
    return terrainTileResult_[idx];
}

void glimmer::TerrainResult::SetTerrainTileStructure(int tileIndex, const ResourceRef *structureResource) {
    if (tileIndex >= 0 && tileIndex < CHUNK_AREA) {
        TerrainTileResult &terrainTileResult = terrainTileResult_[tileIndex];
        terrainTileResult.terrainType = STRUCTURE;
        terrainTileResult.resRef = *structureResource;
    }
}

void glimmer::TerrainResult::SetLeftTerrainTileResult(const int y, const TerrainTileResult &result) {
    leftTerrainTileResult_[y] = result;
}

void glimmer::TerrainResult::SetRightTerrainTileResult(const int y, const TerrainTileResult &result) {
    rightTerrainTileResult_[y] = result;
}

void glimmer::TerrainResult::SetUpTerrainTileResult(const int x, const TerrainTileResult &result) {
    upTerrainTileResult_[x] = result;
}

void glimmer::TerrainResult::SetDownTerrainTileResult(const int x, const TerrainTileResult &result) {
    downTerrainTileResult_[x] = result;
}
