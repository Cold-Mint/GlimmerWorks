//
// Created by Cold-Mint on 2026/3/2.
//

#include "MiningRangeData.h"

#include "core/world/Tile.h"

glimmer::MiningRangeData::MiningRangeData() = default;

const std::vector<WorldVector2D> &glimmer::MiningRangeData::GetPoints() const {
    return points_;
}

float glimmer::MiningRangeData::GetMaxHardness() const {
    return maxHardness_;
}

void glimmer::MiningRangeData::Reset() {
    points_.clear();
    maxHardness_ = 0.0f;
}

void glimmer::MiningRangeData::CalculateMining(WorldVector2D startVector,
                                               const TileLayerComponent *tileLayerComponent) {
    const TileVector2D startPosition = TileLayerComponent::WorldToTile(startVector);
    const Tile *startTile = tileLayerComponent->GetTile(startPosition);
    if (!startTile->IsBreakable()) {
        return;
    }
    points_.emplace_back(startVector);
    maxHardness_ = startTile->GetHardness();
}


void glimmer::MiningRangeData::CalculateChainMining(WorldVector2D startVector,
                                                    const TileLayerComponent *tileLayerComponent,
                                                    const int radius) {
    if (radius <= 0) {
        //Invalid chain radius.
        //无效的连锁半径。
        return;
    }
    const TileVector2D startPosition = TileLayerComponent::WorldToTile(startVector);
    Tile *startTile = tileLayerComponent->GetTile(startPosition);
    if (!startTile->IsBreakable()) {
        return;
    }
    if (startTile->IsPlayerPlaced()) {
        return;
    }
    if (!startTile->IsAllowChainMining()) {
        //The excavated blocks do not support consecutive collection.
        //挖掘的方块不支持连锁采集。
        return;
    }
    maxHardness_ = startTile->GetHardness();
    //Add the array of excavation coordinates.
    //加入挖掘坐标数组。
    points_.emplace_back(startVector);

    //Core implementation: Connected region search
    //核心实现：连通区域查找
    //Direction array (up, down, left, right - 4 directions, can be extended to 8 directions)
    //方向数组（上下左右4方向，可扩展为8方向）
    const std::vector<TileVector2D> directions = {
        {1, 0}, // 右
        {-1, 0}, // 左
        {0, 1}, // 下
        {0, -1} // 上
    };

    //The set of visited blocks (to avoid redundant processing)
    //已访问的方块集合（避免重复处理）
    std::unordered_set<TileVector2D, Vector2DIHash> visited;
    visited.insert(startPosition);

    //Breadth-First Search (BFS) Queue
    //广度优先搜索（BFS）队列
    std::queue<TileVector2D> bfsQueue;
    bfsQueue.push(startPosition);

    //Traverse all connected blocks
    //遍历所有连通方块
    while (!bfsQueue.empty()) {
        TileVector2D currentPos = bfsQueue.front();
        bfsQueue.pop();

        //Traverse all directions
        //遍历所有方向
        for (const auto &dir: directions) {
            TileVector2D nextPos = {currentPos.x + dir.x, currentPos.y + dir.y};

            //Check 1: Is it within the radius range (Manhattan distance/Euclidean distance; here, Manhattan distance is more appropriate for the sub-game)?
            // 检查1：是否在半径范围内（曼哈顿距离/欧几里得距离，这里用曼哈顿更贴合格子游戏）
            int distance = abs(nextPos.x - startPosition.x) + abs(nextPos.y - startPosition.y);
            if (distance > radius) {
                continue;
            }

            // Check 2: Have you visited before?
            // 检查2：是否已访问过
            if (visited.contains(nextPos)) {
                continue;
            }

            //Check 3: Obtain the target block and verify its validity
            //检查3：获取目标方块并验证有效性
            Tile *nextTile = tileLayerComponent->GetTile(nextPos);
            if (nextTile == nullptr || nextTile->IsPlayerPlaced() || !nextTile->IsBreakable() || !nextTile->
                IsAllowChainMining()) {
                continue;
            }

            //All conditions met, add to the result set
            //所有条件满足，加入结果集
            visited.insert(nextPos);
            bfsQueue.push(nextPos);

            //Convert to world coordinates and add "points_"
            //转换为世界坐标并加入points_
            WorldVector2D nextWorldVec = TileLayerComponent::TileToWorld(nextPos);
            points_.emplace_back(nextWorldVec);

            //Update the maximum hardness value
            //更新最大硬度值
            if (nextTile->GetHardness() > maxHardness_) {
                maxHardness_ = nextTile->GetHardness();
            }
        }
    }
}
