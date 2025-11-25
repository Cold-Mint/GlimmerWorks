//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILELAYERCOMPONENT_H
#define GLIMMERWORKS_TILELAYERCOMPONENT_H
#include <optional>
#include <unordered_map>
#include <vector>

#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "SDL3/SDL_rect.h"
#include "../../world/Tile.h"
using TileVector2D = glimmer::Vector2DI;

namespace glimmer {
    class TileLayerComponent final : public GameComponent {
    public:
        /**
         *Tile To World
         * 瓦片坐标转世界坐标
         * @param tileLayerPos tileLayerPos 瓦片层的对象位置
         * @param tilePos TileVector2D 瓦片坐标
         * @return
         */
        [[nodiscard]] static WorldVector2D TileToWorld(const WorldVector2D &tileLayerPos, const TileVector2D &tilePos);

        /**
         * WorldToTile
         * 世界坐标转瓦片坐标
         * @param tileLayerPos tileLayerPos 瓦片层的对象位置
         * @param worldPos worldPos 世界坐标
         * @return
         */
        [[nodiscard]] static TileVector2D WorldToTile(const WorldVector2D &tileLayerPos, const WorldVector2D &worldPos);

        /**
         * Get Tiles In Viewport
         * 获取视口矩形内的所有瓦片数据
         * @param worldViewport
         * @return
         */
        [[nodiscard]] std::vector<std::pair<TileVector2D, Tile> > GetTilesInViewport(const WorldVector2D &tileLayerPos,
            const SDL_FRect &worldViewport) const;

        /**
         * Set Tile
         * 设置瓦片
         * @param tilePos tilePos 瓦片坐标
         * @param tile tile 瓦片
         */
        void SetTile(const TileVector2D &tilePos, const Tile &tile);

        /**
         * Clear Tile
         * 清除指定位置的区块
         * @param tilePos
         */
        void ClearTile(const TileVector2D &tilePos);

        /**
         * GetTile
         * 获取瓦片
         * @param tilePos 瓦片坐标
         * @return Tile 瓦片信息
         */
        [[nodiscard]] std::optional<Tile> GetTile(const TileVector2D &tilePos) const;

    private:
        /**
        * Implement sparse storage using unordered_map: The key is the tile coordinate
        * 使用unordered_map实现稀疏存储：key是瓦片坐标
         */
        std::unordered_map<long long, Tile> tileMap_;

        // 辅助方法：把(x,y)编码成唯一的key
        [[nodiscard]] static long long EncodeTileKey(int x, int y);
    };
}


#endif //GLIMMERWORKS_TILELAYERCOMPONENT_H
