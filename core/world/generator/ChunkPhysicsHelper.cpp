//
// Created by Cold-Mint on 2025/11/9.
//

#include "ChunkPhysicsHelper.h"

#include "box2d/box2d.h"
#include "TilePhysicsType.h"
#include "../../utils/Box2DUtils.h"
#include "box2d/types.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"


void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunk(AppContext *appContext, b2WorldId worldId, Chunk *chunk) {
    if (appContext == nullptr || chunk == nullptr) {
        return;
    }
    LogCat::d("AttachPhysicsBodyToChunk");
    appContext->AddMainThreadTask([worldId,chunk] {
        const TileVector2D chunkPos = chunk->GetPosition();
        std::vector isStaticTile(CHUNK_AREA, false);
        for (int idx = 0; idx < CHUNK_AREA; ++idx) {
            auto tile = chunk->GetTile(Ground, idx);
            if (tile && tile->GetTilePhysicsType() == TilePhysicsType::Static) {
                isStaticTile[idx] = true;
            }
        }
        std::vector visited(CHUNK_AREA, false);
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int x = 0; x < CHUNK_SIZE; ++x) {
                const int baseIdx = y << CHUNK_SHIFT | x;
                if (visited[baseIdx] || !isStaticTile[baseIdx]) {
                    continue;
                }
                int w = 1;
                while (x + w < CHUNK_SIZE) {
                    const int checkIdx = y << CHUNK_SHIFT | x + w;
                    if (visited[checkIdx] || !isStaticTile[checkIdx]) {
                        break;
                    }
                    w++;
                }

                int h = 1;
                bool canExpand;
                do {
                    canExpand = true;
                    const int nextY = y + h;
                    if (nextY >= CHUNK_SIZE) break;

                    for (int k = 0; k < w; ++k) {
                        const int checkIdx = nextY << CHUNK_SHIFT | x + k;
                        if (visited[checkIdx] || !isStaticTile[checkIdx]) {
                            canExpand = false;
                            break;
                        }
                    }

                    if (canExpand) {
                        h++;
                    }
                } while (canExpand);

                for (int j = 0; j < h; ++j) {
                    const int rowOffset = (y + j) << CHUNK_SHIFT;
                    for (int i = 0; i < w; ++i) {
                        visited[rowOffset + x + i] = true;
                    }
                }

                const float localCenterX = static_cast<float>(x) + static_cast<float>(w - 1) * 0.5F;
                const float localCenterY = static_cast<float>(y) + static_cast<float>(h - 1) * 0.5F;
                const float worldX = (static_cast<float>(chunkPos.x) + localCenterX) * TILE_SIZE;
                const float worldY = (static_cast<float>(chunkPos.y) + localCenterY) * TILE_SIZE;
                const WorldVector2D worldPos = {worldX, worldY};
                const auto b2BodyId = CreateStaticBody(worldId, worldPos, {w, h});
                chunk->AddBodyId(b2BodyId);
            }
        }
    });
}

b2BodyId glimmer::ChunkPhysicsHelper::CreateStaticBody(const b2WorldId worldId, const WorldVector2D pos,
                                                       const Vector2DI size) {
    auto bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = b2_staticBody;
    bodyDef_.position = b2Vec2(Box2DUtils::ToMeters(pos.x), Box2DUtils::ToMeters(pos.y));
    auto bodyId_ = b2CreateBody(worldId, &bodyDef_);
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

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunk(AppContext *appContext, Chunk *chunk) {
    if (appContext == nullptr || chunk == nullptr) {
        return;
    }
    LogCat::d("DetachPhysicsBodyToChunk");
    appContext->AddMainThreadTask([chunk] {
        for (const b2BodyId bodyId: chunk->GetAttachedBodies()) {
            if (b2Body_IsValid(bodyId)) {
                b2DestroyBody(bodyId);
            }
        }
        chunk->ClearAttachedBodies();
    });
}

void glimmer::ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(const WorldContext *worldContext, Chunk *chunk) {
    AppContext *appContext = worldContext->GetAppContext();
    DetachPhysicsBodyToChunk(appContext, chunk);
    AttachPhysicsBodyToChunk(appContext, worldContext->GetWorldId(), chunk);
}
