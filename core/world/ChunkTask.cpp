//
// Created by coldmint on 2026/5/13.
//

#include "ChunkTask.h"

uint64_t glimmer::ChunkTask::GetId() const {
    const uint64_t type = static_cast<uint64_t>(chunkType) & 0b11;
    const uint64_t x = static_cast<uint32_t>(tileVector2D.x);
    const uint64_t y = static_cast<uint32_t>(tileVector2D.y);
    return type << 62
           | (x & 0x7FFFFFFFLL) << 31
           | y & 0x7FFFFFFFLL;
}
