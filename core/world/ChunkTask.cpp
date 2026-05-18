//
// Created by coldmint on 2026/5/13.
//

#include "ChunkTask.h"

glimmer::ChunkTask::ChunkTask(const ChunkType chunkType, const TileVector2D chunkVertexCoordinates) {
    chunkType_ = chunkType;
    chunkVertexCoordinates_ = chunkVertexCoordinates;
}

glimmer::ChunkType glimmer::ChunkTask::GetChunkType() const {
    return chunkType_;
}

uint32_t glimmer::ChunkTask::GetDistance() const {
    return distance_;
}

void glimmer::ChunkTask::SetOrigin(const TileVector2D origin) {
    distance_ = origin.DistanceSquared(chunkVertexCoordinates_);
}


const TileVector2D &glimmer::ChunkTask::GetChunkVertexCoordinates() const {
    return chunkVertexCoordinates_;
}


uint64_t glimmer::ChunkTask::GetSignature() const {
    const uint64_t type = static_cast<uint64_t>(chunkType_) & 0b11;
    const uint64_t x = static_cast<uint32_t>(chunkVertexCoordinates_.x);
    const uint64_t y = static_cast<uint32_t>(chunkVertexCoordinates_.y);
    return type << 62
           | (x & 0x7FFFFFFFLL) << 31
           | y & 0x7FFFFFFFLL;
}
