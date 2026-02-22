//
// Created by coldmint on 2026/2/4.
//

#include "TerrainResult.h"

void glimmer::TerrainResult::SetTerrainTileResult(const int x, const int y, const TerrainTileResult &result) {
    const int idx = y * CHUNK_SIZE + x;
    terrainTileResult_[idx] = result;
}

void glimmer::TerrainResult::SetPosition(const TileVector2D &position) {
    position_ = position;
}

TileVector2D glimmer::TerrainResult::GetPosition() const {
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

void glimmer::TerrainResult::MarkStructure(const int index) {
    if (index >= 0 && index < CHUNK_SIZE) {
        terrainTileResult_[index].terrainType = STRUCTURE;
    }
}

void glimmer::TerrainResult::MarkStructureSource(const int index) {
    if (index >= 0 && index < CHUNK_SIZE) {
        terrainTileResult_[index].terrainType = STRUCTURE_SOURCE;
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
