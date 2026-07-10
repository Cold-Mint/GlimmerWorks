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
#include "ChunkTask.h"

#include <utility>

glimmer::ChunkTask::ChunkTask(const ChunkType chunkType, const TileVector2D& chunkVertexCoordinates)
    : chunkType_(chunkType), chunkVertexCoordinates_(chunkVertexCoordinates)
{
}

glimmer::ChunkType glimmer::ChunkTask::GetChunkType() const
{
    return chunkType_;
}

uint32_t glimmer::ChunkTask::GetDistance() const
{
    return distance_;
}

void glimmer::ChunkTask::SetOrigin(const TileVector2D& origin)
{
    distance_ = origin.DistanceSquared(chunkVertexCoordinates_);
}


const glimmer::TileVector2D& glimmer::ChunkTask::GetChunkVertexCoordinates() const
{
    return chunkVertexCoordinates_;
}

uint64_t glimmer::ChunkTask::GetFingerprint() const
{
    const uint64_t type = static_cast<uint64_t>(std::to_underlying(chunkType_)) & 0x3ULL;
    const uint64_t x = static_cast<uint64_t>(chunkVertexCoordinates_.x) & 0x7FFFFFFFULL;
    const uint64_t y = static_cast<uint64_t>(chunkVertexCoordinates_.y) & 0x7FFFFFFFULL;
    return type << 62 | x << 31 | y;
}
