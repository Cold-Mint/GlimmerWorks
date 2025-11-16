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

    //Merge static tiles (Greedy maximum rectangle)
    //合并静态 tile（贪心最大矩形）
    for (int startX = 0; startX < CHUNK_SIZE; startX++)
    {
        for (int startY = 0; startY < CHUNK_SIZE; startY++)
        {
            if (visited[startX][startY])
            {
                continue;
            }

            auto tile = chunk->GetTile(startX, startY);
            if (tile.physicsType != TilePhysicsType::Static)
            {
                continue;
            }

            int maxWidth = 0;
            int maxHeight = 0;

            while (startX + maxWidth < CHUNK_SIZE)
            {
                auto t = chunk->GetTile(startX + maxWidth, startY);
                if (t.physicsType == TilePhysicsType::Static)
                    maxWidth++;
                else
                    break;
            }

            bool canExtend = true;
            while (startY + maxHeight < CHUNK_SIZE && canExtend)
            {
                for (int w = 0; w < maxWidth; w++)
                {
                    auto t = chunk->GetTile(startX + w, startY + maxHeight);
                    if (t.physicsType != TilePhysicsType::Static)
                    {
                        canExtend = false;
                        break;
                    }
                }
                if (canExtend)
                {
                    maxHeight++;
                }
            }

            for (int x = 0; x < maxWidth; x++)
            {
                for (int y = 0; y < maxHeight; y++)
                {
                    visited[startX + x][startY + y] = true;
                }
            }

            auto tilePos = TileVector2D{chunkPos.x + startX, chunkPos.y + startY};
            CreateStaticBody(worldId,
                             {TileLayerComponent::TileToWorld(tileLayerPos, tilePos),},
                             {maxWidth, maxHeight});
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
