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
#include "ChunkPhysicsHelper.h"

#include "box2d/box2d.h"
#include "TilePhysicsType.h"
#include "core/log/LogCat.h"
#include "core/utils/Box2DUtils.h"
#include "core/world/PhysicsTaskQueue.h"
#include "core/world/WorldContext.h"
#include "box2d/types.h"
#include "core/math/CoordinateTransformer.h"
#include "core/world/Tile.h"


std::vector<bool> glimmer::ChunkPhysicsHelper::CollectStaticTiles(const Chunk *chunk) {
    std::vector isStaticTile(CHUNK_AREA, false);
    for (int idx = 0; idx < CHUNK_AREA; ++idx) {
        const auto tile = chunk->GetTile(TileLayerType::Ground, idx);
        if (tile != nullptr && tile->GetTilePhysicsType() == TilePhysicsType::Static) {
            isStaticTile[idx] = true;
        }
    }
    return isStaticTile;
}

glimmer::Vector2DI glimmer::ChunkPhysicsHelper::FindRectSize(const int startX, const int startY,
                                                             const std::vector<bool> &isStaticTile,
                                                             std::vector<bool> &visited) {
    int width = 1;
    while (startX + width < CHUNK_SIZE) {
        const size_t checkIdx = CoordinateTransformer::GetArrayIndex(startX + width, startY, CHUNK_SHIFT);
        if (visited[checkIdx] || !isStaticTile[checkIdx]) {
            break;
        }
        width++;
    }

    int height = 1;
    bool canExpand;
    do {
        canExpand = true;
        const int nextY = startY + height;
        if (nextY >= CHUNK_SIZE) {
            break;
        }
        for (int k = 0; k < width; ++k) {
            const size_t checkIdx = CoordinateTransformer::GetArrayIndex(startX + k, nextY, CHUNK_SHIFT);
            if (visited[checkIdx] || !isStaticTile[checkIdx]) {
                canExpand = false;
                break;
            }
        }
        if (canExpand) {
            height++;
        }
    } while (canExpand);

    return {width, height};
}

void glimmer::ChunkPhysicsHelper::MarkVisited(const int startX, const int startY, const int width, const int height,
                                              std::vector<bool> &visited) {
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            visited[CoordinateTransformer::GetArrayIndex(startX + i, startY + j, CHUNK_SHIFT)] = true;
        }
    }
}

std::vector<glimmer::ChunkPhysicsRect> glimmer::ChunkPhysicsHelper::CollectRects(const std::shared_ptr<Chunk> &chunk) {
    std::vector<ChunkPhysicsRect> rects;
    const Chunk *chunkPtr = chunk.get();
    if (chunkPtr == nullptr) {
        return rects;
    }
    const std::vector<bool> isStaticTile = CollectStaticTiles(chunkPtr);
    std::vector visited(CHUNK_AREA, false);
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            if (const size_t baseIdx = CoordinateTransformer::GetArrayIndex(x, y, CHUNK_SHIFT);
                visited[baseIdx] || !isStaticTile[baseIdx]) {
                continue;
            }
            const Vector2DI rectSize = FindRectSize(x, y, isStaticTile, visited);
            MarkVisited(x, y, rectSize.x, rectSize.y, visited);
            rects.push_back({x, y, rectSize.x, rectSize.y});
        }
    }
    return rects;
}

void glimmer::ChunkPhysicsHelper::CreateBodyForRect(const b2WorldId worldId, Chunk *chunk,
                                                    const ChunkPhysicsRect &rect) {
    const WorldVector2D topLeftWorld = CoordinateTransformer::TileToWorld(
        CoordinateTransformer::ChunkRelativeToTile(chunk->GetPosition(), ChunkRelativeVector2D(rect.x, rect.y)));
    const WorldVector2D worldPos = topLeftWorld + WorldVector2D{
                                       static_cast<float>(rect.width - 1) * HALF_TILE_SIZE,
                                       static_cast<float>(rect.height - 1) * HALF_TILE_SIZE
                                   };
    const auto b2BodyId = CreateStaticBody(worldId, worldPos, {rect.width, rect.height});
    chunk->AddBodyId(b2BodyId);
}

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunkNow(const std::shared_ptr<Chunk> &chunk) {
    if (chunk == nullptr) {
        return;
    }
    const std::vector<b2BodyId> &bodies = chunk->GetAttachedBodies();
    for (const b2BodyId bodyId: bodies) {
        if (b2Body_IsValid(bodyId)) {
            b2DestroyBody(bodyId);
        }
    }
    chunk->ClearAttachedBodies();
}

void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunkNow(b2WorldId worldId, const std::shared_ptr<Chunk>& chunk,
                                                              const std::vector<ChunkPhysicsRect> &rects) {
    Chunk *chunkPtr = chunk.get();
    for (const ChunkPhysicsRect &rect: rects) {
        CreateBodyForRect(worldId, chunkPtr, rect);
    }
}

b2BodyId glimmer::ChunkPhysicsHelper::CreateStaticBody(const b2WorldId worldId, const WorldVector2D &pos,
                                                       const Vector2DI &size) {
    auto bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = b2_staticBody;
    bodyDef_.position = b2Vec2(Box2DUtils::ToMeters(pos.x), Box2DUtils::ToMeters(pos.y));
    const auto bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F;
    shapeDef.material.friction = 0.3F;
    b2Filter filter{};
    filter.categoryBits = BOX2D_CATEGORY_TILE;
    filter.maskBits = BOX2D_CATEGORY_PLAYER | BOX2D_CATEGORY_ITEM;
    shapeDef.filter = filter;
    const b2Polygon bodyPolygon = b2MakeBox(Box2DUtils::ToMeters(static_cast<float>(size.x * TILE_SIZE) / 2),
                                            Box2DUtils::ToMeters(static_cast<float>(size.y * TILE_SIZE) / 2));

    b2CreatePolygonShape(bodyId_, &shapeDef, &bodyPolygon);
    return bodyId_;
}

void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunk(const WorldContext *worldContext,
                                                           const std::shared_ptr<Chunk> &chunk) {
    if (worldContext == nullptr || chunk == nullptr) {
        return;
    }
    LogCat::d(LogLabel::CHUNK, "chunk_physics_attach", "Attaching physics body to chunk: position=({}, {})",
              chunk->GetPosition().x, chunk->GetPosition().y);
    PhysicsTaskQueue *taskQueue = worldContext->GetPhysicsTaskQueue();
    if (taskQueue == nullptr) {
        return;
    }
    const std::vector<ChunkPhysicsRect> rects = CollectRects(chunk);
    const b2WorldId worldId = worldContext->GetWorldId();
    taskQueue->Enqueue([chunk, worldId, rects] {
        AttachPhysicsBodyToChunkNow(worldId, chunk, rects);
    });
}

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunk(const WorldContext *worldContext,
                                                           const std::shared_ptr<Chunk> &chunk) {
    if (worldContext == nullptr || chunk == nullptr) {
        return;
    }
    LogCat::d(LogLabel::CHUNK, "chunk_physics_detach", "Detaching physics body from chunk: position=({}, {})",
              chunk->GetPosition().x, chunk->GetPosition().y);
    PhysicsTaskQueue *taskQueue = worldContext->GetPhysicsTaskQueue();
    if (taskQueue == nullptr) {
        return;
    }
    taskQueue->Enqueue([chunk] {
        DetachPhysicsBodyToChunkNow(chunk);
    });
}

void glimmer::ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(const WorldContext *worldContext,
                                                           const std::shared_ptr<Chunk> &chunk) {
    if (worldContext == nullptr || chunk == nullptr) {
        return;
    }
    LogCat::d(LogLabel::CHUNK, "chunk_physics_update", "Updating physics body for chunk: position=({}, {})",
              chunk->GetPosition().x, chunk->GetPosition().y);
    PhysicsTaskQueue *taskQueue = worldContext->GetPhysicsTaskQueue();
    if (taskQueue == nullptr) {
        return;
    }
    const std::vector<ChunkPhysicsRect> rects = CollectRects(chunk);
    const b2WorldId worldId = worldContext->GetWorldId();
    taskQueue->Enqueue([chunk, worldId, rects] {
        DetachPhysicsBodyToChunkNow(chunk);
        AttachPhysicsBodyToChunkNow(worldId, chunk, rects);
    });
}
