//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILELAYERCOMPONENT_H
#define GLIMMERWORKS_TILELAYERCOMPONENT_H
#include <memory>
#include <unordered_map>
#include <vector>

#include "../GameComponent.h"
#include "../../math/Vector2D.h"
#include "../../math/Vector2DI.h"
#include "core/world/generator/TileLayerType.h"
#include "SDL3/SDL_rect.h"
using TileVector2D = glimmer::Vector2DI;

namespace glimmer {
    class Chunk;
    class Tile;
    /**
     * The TileLayer component is designed to operate only at the world origin (0,0).
     * The position of the Transform2DComponent does not affect the coordinate transformation.
     * TileLayer组件被设计为只能在世界原点（0,0）工作。
     * Transform2DComponent组件的位置不会影响坐标转换。
     */
    class TileLayerComponent final : public GameComponent {
        [[nodiscard]] Tile *GetTile(TileLayerType layerType, const TileVector2D &tilePos) const;

        /**
         * GetTopVisibleTile
         * 获取顶层可见瓦片
         * @param layerFilter
         * @param tilePos
         * @return
         */
        [[nodiscard]] std::vector<Tile *> GetTopVisibleTiles(uint8_t layerFilter, const TileVector2D &tilePos) const;

    public:
        /**
         *Tile To World
         * 瓦片坐标转世界坐标
         * @param tilePos TileVector2D 瓦片坐标
         * @return WorldVector2D(The upper left corner of the tile) 世界坐标（瓦片左上角）
         */
        [[nodiscard]] static WorldVector2D TileToWorld(const TileVector2D &tilePos);

        /**
        * Tile To World
         * 瓦片坐标转世界坐标
         * @param tilePos TileVector2D 瓦片坐标
         * @return WorldVector2D(The center of the tile) 世界坐标（瓦片中心）
         */
        [[nodiscard]] static WorldVector2D TileToWorldCenter(const TileVector2D &tilePos);

        /**
         * WorldToTile
         * 世界坐标转瓦片坐标
         * @param worldPos worldPos 世界坐标
         * @return
         */
        [[nodiscard]] static TileVector2D WorldToTile(const WorldVector2D &worldPos);

        /**
         * Get Top Visible Tiles In Viewport
         * 获取视口矩形内的所有顶层可见瓦片数据
         * @param layerFilter The layer to be queried 要查询的图层
         * @param worldViewport
         * @return
         */
        [[nodiscard]] std::vector<std::pair<TileVector2D, std::vector<Tile *> > > GetTopVisibleTilesInViewport(
            uint8_t layerFilter,
            const SDL_FRect &worldViewport) const;

        /**
         * Set Tile
         * 设置瓦片
         * @param tilePos tilePos 瓦片坐标
         * @param tile tile 瓦片
         * @return If the block to which the tile belongs has not yet been loaded, return false 如果放置瓦片所属区块尚未加载，则返回false
         */
        [[nodiscard]] bool SetTile(const TileVector2D &tilePos, std::unique_ptr<Tile> tile) const;

        /**
         * GetSelfLayerTile
         * 获取子身图层瓦片的信息
         * @param tilePos 瓦片坐标
         * @return Tile 瓦片信息
         */
        [[nodiscard]] Tile *GetSelfLayerTile(const TileVector2D &tilePos) const;

        [[nodiscard]] std::unique_ptr<Tile> ReplaceTile(const TileVector2D &tileVector2d,
                                                        std::unique_ptr<Tile> newTile) const;


        [[nodiscard]] TileLayerType GetTileLayerType() const;

        /**
         * Set Focus Position
         * 设置焦点位置
         * @param focusPosition
         */
        void SetFocusPosition(TileVector2D focusPosition);

        /**
         * Get the focus position
         * 获取焦点位置
         * @return
         */
        [[nodiscard]] const TileVector2D &GetFocusPosition() const;

        explicit TileLayerComponent(const TileLayerType tileLayerType,
                                    std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *
                                    chunks) : chunks_(chunks),
                                              tileLayerType_(tileLayerType) {
        }

        [[nodiscard]] uint32_t GetId() override;

    private:
        std::unordered_map<TileVector2D, Chunk *, Vector2DIHash> *chunks_;

        TileLayerType tileLayerType_;
        TileVector2D focusPosition_ = TileVector2D{};
    };
}


#endif //GLIMMERWORKS_TILELAYERCOMPONENT_H
