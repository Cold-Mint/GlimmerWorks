//
// Created by Cold-Mint on 2025/11/9.
//

#include "ChunkPhysicsHelper.h"

#include "../ecs/component/TileLayerComponent.h"
#include "../utils/Box2DUtils.h"
#include "box2d/box2d.h"
#include "box2d/types.h"


void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunk(const b2WorldId worldId, const WorldVector2D& tileLayerPos,
                                                           Chunk* chunk)
{
    std::vector<TileVector2D> dynamicTiles;
    bool visited[CHUNK_SIZE][CHUNK_SIZE] = {false};
    TileVector2D chunkPos = chunk->GetPosition();

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            auto tile = chunk->GetTile(x, y);
            if (tile.physicsType == TilePhysicsType::Dynamic)
            {
                dynamicTiles.emplace_back(chunkPos.x + x, chunkPos.y + y);
            }
        }
    }

    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
            auto tile = chunk->GetTile(x, y);
            if (tile.physicsType != TilePhysicsType::Static)
                continue;
            TileVector2D tilePos(chunkPos.x + x, chunkPos.y + y);
            WorldVector2D worldPos =
                TileLayerComponent::TileToWorld(tileLayerPos, tilePos);
            CreateStaticBody(worldId,
                             worldPos,
                             {1, 1});
        }
    }

    for (auto& pos : dynamicTiles)
    {
        CreateDynamicTileBody(chunk, pos);
    }
}

void glimmer::ChunkPhysicsHelper::CreateStaticBody(b2WorldId worldId, WorldVector2D pos,
                                                   Vector2DI size)
{
    auto bodyDef_ = b2DefaultBodyDef();
    bodyDef_.type = b2_staticBody;
    bodyDef_.position = b2Vec2(Box2DUtils::ToMeters(pos.x), Box2DUtils::ToMeters(pos.y));
    auto bodyId_ = b2CreateBody(worldId, &bodyDef_);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;
    const b2Polygon bodyPolygon = b2MakeBox(Box2DUtils::ToMeters(static_cast<float>(size.x * TILE_SIZE) / 2),
                                            Box2DUtils::ToMeters(static_cast<float>(size.y * TILE_SIZE) / 2));
    b2CreatePolygonShape(bodyId_, &shapeDef, &bodyPolygon);
}


void glimmer::ChunkPhysicsHelper::CreateDynamicTileBody(Chunk* chunk, TileVector2D pos)
{
}

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunk(Chunk* chunk)
{
}
