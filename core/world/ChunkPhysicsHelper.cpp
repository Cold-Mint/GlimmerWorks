//
// Created by Cold-Mint on 2025/11/9.
//

#include "ChunkPhysicsHelper.h"

#include "../ecs/component/TileLayerComponent.h"
#include "../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "box2d/types.h"


void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunk(const b2WorldId worldId, const WorldVector2D &tileLayerPos,
                                                           Chunk *chunk) {
    std::vector<TileVector2D> dynamicTiles;
    TileVector2D chunkPos = chunk->GetPosition();

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            auto tile = chunk->GetTile(TileLayerType::Main, x, y);
            if (tile.physicsType == TilePhysicsType::Dynamic) {
                dynamicTiles.emplace_back(chunkPos.x + x, chunkPos.y + y);
            }
        }
    }
    std::array<std::array<bool, CHUNK_SIZE>, CHUNK_SIZE> visited{};
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            if (visited[x][y]) continue;

            auto tile = chunk->GetTile(TileLayerType::Main, x, y);
            if (tile.physicsType != TilePhysicsType::Static) continue;

            // Start a new rectangle
            int w = 1;
            int h = 1;

            // Expand width
            while (x + w < CHUNK_SIZE) {
                if (visited[x + w][y]) break;
                auto nextTile = chunk->GetTile(TileLayerType::Main, x + w, y);
                if (nextTile.physicsType != TilePhysicsType::Static) break;
                w++;
            }

            // Expand height
            bool canExpandHeight = true;
            while (y + h < CHUNK_SIZE) {
                for (int k = 0; k < w; k++) {
                    if (visited[x + k][y + h]) {
                        canExpandHeight = false;
                        break;
                    }
                    auto nextTile = chunk->GetTile(TileLayerType::Main, x + k, y + h);
                    if (nextTile.physicsType != TilePhysicsType::Static) {
                        canExpandHeight = false;
                        break;
                    }
                }
                if (!canExpandHeight) break;
                h++;
            }

            // Mark visited
            for (int i = 0; i < w; i++) {
                for (int j = 0; j < h; j++) {
                    visited[x + i][y + j] = true;
                }
            }

            // Create body
            // Center in local chunk coords (float)
            float localCenterX = static_cast<float>(x) + static_cast<float>(w - 1) * 0.5F;
            float localCenterY = static_cast<float>(y) + static_cast<float>(h - 1) * 0.5F;

            // Convert to world coords
            const float worldX = (static_cast<float>(chunkPos.x) + localCenterX) * TILE_SIZE + tileLayerPos.x;
            const float worldY = (static_cast<float>(chunkPos.y) + localCenterY) * TILE_SIZE + tileLayerPos.y;

            const WorldVector2D worldPos = {worldX, worldY};

            const auto b2BodyId = CreateStaticBody(worldId, worldPos, {w, h});
            chunk->AddBodyId(b2BodyId);
        }
    }

    for (auto &pos: dynamicTiles) {
        CreateDynamicTileBody(chunk, pos);
    }
}

b2BodyId glimmer::ChunkPhysicsHelper::CreateStaticBody(b2WorldId worldId, WorldVector2D pos,
                                                       Vector2DI size) {
    auto bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = b2_staticBody;
    bodyDef_.position = b2Vec2(Box2DUtils::ToMeters(pos.x), Box2DUtils::ToMeters(pos.y));
    auto bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0F;
    shapeDef.material.friction = 0.3F;
    const b2Polygon bodyPolygon = b2MakeBox(Box2DUtils::ToMeters(static_cast<float>(size.x * TILE_SIZE) / 2),
                                            Box2DUtils::ToMeters(static_cast<float>(size.y * TILE_SIZE) / 2));
    b2CreatePolygonShape(bodyId_, &shapeDef, &bodyPolygon);
    return bodyId_;
}


void glimmer::ChunkPhysicsHelper::CreateDynamicTileBody(Chunk *chunk, TileVector2D pos) {
}

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunk(Chunk *chunk) {
    if (chunk == nullptr) {
        return;
    }

    for (const b2BodyId bodyId: chunk->GetAttachedBodies()) {
        if (b2Body_IsValid(bodyId)) {
            b2DestroyBody(bodyId);
        }
    }
    chunk->ClearAttachedBodies();
}
