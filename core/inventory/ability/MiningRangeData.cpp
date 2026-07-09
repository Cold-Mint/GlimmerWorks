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
    auto offset = TileVector2D(0, 0);
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
    maxHardness_ = 0.0F;
}

void glimmer::MiningRangeData::CalculateMining(const TileLayerComponent* tileLayerComponent,
                                               const TileVector2D& startVector)
{
    const Tile* startTile = tileLayerComponent->GetSelfLayerTile(startVector);
    if (startTile == nullptr)
    {
        return;
    }
    const TileMiningData* tileMiningData = startTile->GetMiningData();
    if (tileMiningData == nullptr)
    {
        return;
    }
    if (!tileMiningData->IsBreakable())
    {
        return;
    }
    const TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileState(startVector);
    if (tileStateMessage == nullptr)
    {
        return;
    }

    TryPushPoint(tileLayerComponent, startVector);
    maxHardness_ = tileMiningData->GetHardness();
}

bool glimmer::MiningRangeData::IsValidForChainMining(const TileLayerComponent* tileLayerComponent,
                                                     const TileVector2D& position)
{
    const Tile* tile = tileLayerComponent->GetSelfLayerTile(position);
    if (tile == nullptr)
    {
        return false;
    }
    const TileMiningData* tileMiningData = tile->GetMiningData();
    if (tileMiningData == nullptr)
    {
        return false;
    }
    if (!tileMiningData->IsBreakable() || !tileMiningData->IsAllowChainMining())
    {
        return false;
    }
    const TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileState(position);
    if (tileStateMessage != nullptr && tileStateMessage->placesource() == PLACE_SOURCE_PLAYER)
    {
        return false;
    }
    return true;
}

const glimmer::TileMiningData* glimmer::MiningRangeData::GetValidStartMiningData(const TileLayerComponent* tileLayerComponent,
                                                                                 const TileVector2D& startVector)
{
    const Tile* startTile = tileLayerComponent->GetSelfLayerTile(startVector);
    if (startTile == nullptr)
    {
        return nullptr;
    }
    const TileMiningData* tileMiningData = startTile->GetMiningData();
    if (tileMiningData == nullptr)
    {
        return nullptr;
    }
    if (!tileMiningData->IsBreakable())
    {
        return nullptr;
    }
    const TileStateMessage* tileStateMessage = tileLayerComponent->GetSelfLayerTileState(startVector);
    if (tileStateMessage != nullptr && tileStateMessage->placesource() == PLACE_SOURCE_PLAYER)
    {
        return nullptr;
    }
    if (!tileMiningData->IsAllowChainMining())
    {
        return nullptr;
    }
    return tileMiningData;
}

void glimmer::MiningRangeData::ProcessChainMiningNeighbor(const TileLayerComponent* tileLayerComponent,
                                                          const TileVector2D& nextPos,
                                                          const TileVector2D& startVector,
                                                          uint8_t radius,
                                                          std::unordered_set<Vector2DIFingerprint>& visited,
                                                          std::queue<TileVector2D>& bfsQueue)
{
    int distance = abs(nextPos.x - startVector.x) + abs(nextPos.y - startVector.y);
    if (distance > radius)
    {
        return;
    }

    Vector2DIFingerprint fingerprint = nextPos.GetFingerprint();
    if (visited.contains(fingerprint))
    {
        return;
    }

    if (!IsValidForChainMining(tileLayerComponent, nextPos))
    {
        return;
    }

    visited.insert(fingerprint);
    bfsQueue.push(nextPos);
    TryPushPoint(tileLayerComponent, nextPos);

    const Tile* nextTile = tileLayerComponent->GetSelfLayerTile(nextPos);
    if (nextTile == nullptr)
    {
        return;
    }

    const TileMiningData* nextTileMiningData = nextTile->GetMiningData();
    if (float nextHardness = nextTileMiningData->GetHardness(); nextHardness > maxHardness_)
    {
        maxHardness_ = nextHardness;
    }
}

void glimmer::MiningRangeData::CalculateChainMining(const TileLayerComponent* tileLayerComponent,
                                                    const TileVector2D& startVector, uint8_t radius)
{
    if (radius <= 0)
    {
        return;
    }

    const TileMiningData* tileMiningData = GetValidStartMiningData(tileLayerComponent, startVector);
    if (tileMiningData == nullptr)
    {
        return;
    }

    maxHardness_ = tileMiningData->GetHardness();
    TryPushPoint(tileLayerComponent, startVector);

    const std::vector<TileVector2D> directions = {
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    };

    std::unordered_set<Vector2DIFingerprint> visited;
    visited.insert(startVector.GetFingerprint());

    std::queue<TileVector2D> bfsQueue;
    bfsQueue.push(startVector);

    while (!bfsQueue.empty())
    {
        const TileVector2D& currentPos = bfsQueue.front();
        bfsQueue.pop();

        for (const auto& dir : directions)
        {
            TileVector2D nextPos = {currentPos.x + dir.x, currentPos.y + dir.y};
            ProcessChainMiningNeighbor(tileLayerComponent, nextPos, startVector, radius, visited, bfsQueue);
        }
    }
}
