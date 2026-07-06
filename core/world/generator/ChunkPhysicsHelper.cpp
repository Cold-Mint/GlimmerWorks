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
#include "core/utils/Box2DUtils.h"
#include "box2d/types.h"
#include "core/world/Tile.h"
#include "core/world/WorldContext.h"


std::vector<bool> glimmer::ChunkPhysicsHelper::CollectStaticTiles(const Chunk* chunk)
{
    std::vector<bool> isStaticTile(CHUNK_AREA, false);
    for (int idx = 0; idx < CHUNK_AREA; ++idx)
    {
        const auto tile = chunk->GetTile(Ground, idx);
        if (tile != nullptr && tile->GetTilePhysicsType() == TilePhysicsType::Static)
        {
            isStaticTile[idx] = true;
        }
    }
    return isStaticTile;
}

glimmer::Vector2DI glimmer::ChunkPhysicsHelper::FindRectSize(int startX, int startY,
                                                     const std::vector<bool>& isStaticTile,
                                                     std::vector<bool>& visited)
{
    int width = 1;
    while (startX + width < CHUNK_SIZE)
    {
        const int checkIdx = startY << CHUNK_SHIFT | (startX + width);
        if (visited[checkIdx] || !isStaticTile[checkIdx])
        {
            break;
        }
        width++;
    }

    int height = 1;
    bool canExpand;
    do
    {
        canExpand = true;
        const int nextY = startY + height;
        if (nextY >= CHUNK_SIZE)
        {
            break;
        }
        for (int k = 0; k < width; ++k)
        {
            const int checkIdx = nextY << CHUNK_SHIFT | (startX + k);
            if (visited[checkIdx] || !isStaticTile[checkIdx])
            {
                canExpand = false;
                break;
            }
        }
        if (canExpand)
        {
            height++;
        }
    }
    while (canExpand);

    return {width, height};
}

void glimmer::ChunkPhysicsHelper::MarkVisited(int startX, int startY, int width, int height,
                                             std::vector<bool>& visited)
{
    for (int j = 0; j < height; ++j)
    {
        const int rowOffset = (startY + j) << CHUNK_SHIFT;
        for (int i = 0; i < width; ++i)
        {
            visited[rowOffset + startX + i] = true;
        }
    }
}

void glimmer::ChunkPhysicsHelper::CreateBodyForRect(b2WorldId worldId, Chunk* chunk,
                                                     int x, int y, int width, int height)
{
    const TileVector2D chunkPos = chunk->GetPosition();
    const float localCenterX = static_cast<float>(x) + static_cast<float>(width - 1) * 0.5F;
    const float localCenterY = static_cast<float>(y) + static_cast<float>(height - 1) * 0.5F;
    const float worldX = (static_cast<float>(chunkPos.x) + localCenterX) * TILE_SIZE;
    const float worldY = (static_cast<float>(chunkPos.y) + localCenterY) * TILE_SIZE;
    const WorldVector2D worldPos = {worldX, worldY};
    const auto b2BodyId = CreateStaticBody(worldId, worldPos, {width, height});
    chunk->AddBodyId(b2BodyId);
}

void glimmer::ChunkPhysicsHelper::AttachPhysicsBodyToChunk(AppContext* appContext, b2WorldId worldId, Chunk* chunk)
{
    if (appContext == nullptr || chunk == nullptr)
    {
        return;
    }
    appContext->RunOnMainThread([worldId, chunk]
    {
        const std::vector<bool> isStaticTile = CollectStaticTiles(chunk);
        std::vector<bool> visited(CHUNK_AREA, false);

        for (int y = 0; y < CHUNK_SIZE; ++y)
        {
            for (int x = 0; x < CHUNK_SIZE; ++x)
            {
                const int baseIdx = y << CHUNK_SHIFT | x;
                if (visited[baseIdx] || !isStaticTile[baseIdx])
                {
                    continue;
                }
                const Vector2DI rectSize = FindRectSize(x, y, isStaticTile, visited);
                MarkVisited(x, y, rectSize.x, rectSize.y, visited);
                CreateBodyForRect(worldId, chunk, x, y, rectSize.x, rectSize.y);
            }
        }
    });
}

b2BodyId glimmer::ChunkPhysicsHelper::CreateStaticBody(const b2WorldId worldId, const WorldVector2D pos,
                                                       const Vector2DI size)
{
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

void glimmer::ChunkPhysicsHelper::DetachPhysicsBodyToChunk(AppContext* appContext, Chunk* chunk)
{
    if (appContext == nullptr || chunk == nullptr)
    {
        return;
    }
    appContext->RunOnMainThread([chunk]
    {
        for (const b2BodyId bodyId : chunk->GetAttachedBodies())
        {
            if (b2Body_IsValid(bodyId))
            {
                b2DestroyBody(bodyId);
            }
        }
        chunk->ClearAttachedBodies();
    });
}

void glimmer::ChunkPhysicsHelper::UpdatePhysicsBodyToChunk(const WorldContext* worldContext, Chunk* chunk)
{
    AppContext* appContext = worldContext->GetAppContext();
    DetachPhysicsBodyToChunk(appContext, chunk);
    AttachPhysicsBodyToChunk(appContext, worldContext->GetWorldId(), chunk);
}
