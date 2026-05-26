//
// Created by Cold-Mint on 2026/3/2.
//

#include "MiningRangeData.h"

#include "core/log/LogCat.h"
#include "core/world/Tile.h"
#include "src/saves/tile_state.pb.h"

void glimmer::MiningRangeData::TryPushPoint(const TileLayerComponent* tileLayerComponent,
                                            const TileVector2D& position)
{
    if (tileLayerComponent == nullptr)
    {
        return;
    }
    const TileStateMessage* currentTileStateMessage = tileLayerComponent->GetSelfLayerTileState(position);
    if (currentTileStateMessage == nullptr)
    {
        return;
    }
    auto offset = Vector2DI(0, 0);
    offset.ReadVector2DIMessage(currentTileStateMessage->offset());
    const TileVector2D tileTopLeftPosition = position + offset;
    const TileStateMessage* topLeftTileStateMessage = tileLayerComponent->GetSelfLayerTileState(tileTopLeftPosition);
    if (topLeftTileStateMessage == nullptr)
    {
        return;
    }
    const Vector2DIFingerprint fingerprint = tileTopLeftPosition.GetFingerprint();
    if (pointsFingerprint_.contains(fingerprint))
    {
        return;
    }
    MiningRangeDataPoint miningRangeDataPoint{};
    miningRangeDataPoint.SetWidth(topLeftTileStateMessage->width());
    miningRangeDataPoint.SetHeight(topLeftTileStateMessage->height());
    miningRangeDataPoint.SetTileTopLeftPosition(tileTopLeftPosition);
    points_.emplace_back(miningRangeDataPoint);
    pointsFingerprint_.insert(fingerprint);
}


glimmer::MiningRangeData::MiningRangeData() = default;

size_t glimmer::MiningRangeData::GetPointsCount() const
{
    return points_.size();
}

const glimmer::MiningRangeDataPoint* glimmer::MiningRangeData::GetPoint(const size_t index) const
{
    if (index >= points_.size())
    {
        return nullptr;
    }
    return &points_[index];
}

float glimmer::MiningRangeData::GetMaxHardness() const
{
    return maxHardness_;
}

void glimmer::MiningRangeData::Reset()
{
    points_.clear();
    pointsFingerprint_.clear();
    maxHardness_ = 0.0f;
}

void glimmer::MiningRangeData::CalculateMining(const TileLayerComponent* tileLayerComponent,
                                               const TileVector2D& startVector)
{
    const Tile* startTile = tileLayerComponent->GetSelfLayerTile(startVector);
    if (startTile == nullptr || !startTile->IsBreakable())
    {
        return;
    }
    const TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileState(startVector);
    if (tileStateMessage == nullptr)
    {
        return;
    }

    TryPushPoint(tileLayerComponent, startVector);
    maxHardness_ = startTile->GetHardness();
}

void glimmer::MiningRangeData::CalculateChainMining(const TileLayerComponent* tileLayerComponent,
                                                    const TileVector2D& startVector, uint8_t radius)
{
    if (radius <= 0)
    {
        //Invalid chain radius.
        //无效的连锁半径。
        return;
    }
    const Tile* startTile = tileLayerComponent->GetSelfLayerTile(startVector);
    if (startTile == nullptr)
    {
        return;
    }
    if (!startTile->IsBreakable())
    {
        return;
    }
    const TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileState(startVector);
    if (tileStateMessage != nullptr && tileStateMessage->isplayerplaced())
    {
        return;
    }
    if (!startTile->IsAllowChainMining())
    {
        //The excavated blocks do not support consecutive collection.
        //挖掘的方块不支持连锁采集。
        return;
    }
    maxHardness_ = startTile->GetHardness();
    //Add the array of excavation coordinates.
    //加入挖掘坐标数组。
    TryPushPoint(tileLayerComponent, startVector);

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
    std::unordered_set<Vector2DIFingerprint> visited;
    visited.insert(startVector.GetFingerprint());

    //Breadth-First Search (BFS) Queue
    //广度优先搜索（BFS）队列
    std::queue<TileVector2D> bfsQueue;
    bfsQueue.push(startVector);

    //Traverse all connected blocks
    //遍历所有连通方块
    while (!bfsQueue.empty())
    {
        const TileVector2D& currentPos = bfsQueue.front();
        bfsQueue.pop();

        //Traverse all directions
        //遍历所有方向
        for (const auto& dir : directions)
        {
            TileVector2D nextPos = {currentPos.x + dir.x, currentPos.y + dir.y};

            //Check 1: Is it within the radius range (Manhattan distance/Euclidean distance; here, Manhattan distance is more appropriate for the sub-game)?
            // 检查1：是否在半径范围内（曼哈顿距离/欧几里得距离，这里用曼哈顿更贴合格子游戏）
            int distance = abs(nextPos.x - startVector.x) + abs(nextPos.y - startVector.y);
            if (distance > radius)
            {
                continue;
            }

            Vector2DIFingerprint fingerprint = nextPos.GetFingerprint();
            // Check 2: Have you visited before?
            // 检查2：是否已访问过
            if (visited.contains(fingerprint))
            {
                continue;
            }

            //Check 3: Obtain the target block and verify its validity
            //检查3：获取目标方块并验证有效性
            const Tile* nextTile = tileLayerComponent->GetSelfLayerTile(nextPos);
            if (nextTile == nullptr || !nextTile->IsBreakable() || !nextTile->
                IsAllowChainMining())
            {
                continue;
            }
            const TileStateMessage* nextTileStateMessage = tileLayerComponent->GetSelfLayerTileState(nextPos);
            if (nextTileStateMessage != nullptr && nextTileStateMessage->isplayerplaced())
            {
                continue;
            }

            //All conditions met, add to the result set
            //所有条件满足，加入结果集
            visited.insert(fingerprint);
            bfsQueue.push(nextPos);
            TryPushPoint(tileLayerComponent, nextPos);
            //Update the maximum hardness value
            //更新最大硬度值
            if (nextTile->GetHardness() > maxHardness_)
            {
                maxHardness_ = nextTile->GetHardness();
            }
        }
    }
}
