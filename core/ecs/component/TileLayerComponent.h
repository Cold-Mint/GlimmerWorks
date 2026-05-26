//
// Created by Cold-Mint on 2025/11/2.
//

#pragma once
#include <memory>
#include <vector>

#include "core/ecs/GameComponent.h"
#include "core/math/Vector2D.h"
#include "core/math/Vector2DI.h"
#include "core/world/generator/TileLayerType.h"
#include "core/world/generator/TileSnapshot.h"
#include "SDL3/SDL_rect.h"
using TileVector2D = glimmer::Vector2DI;

namespace glimmer
{
    class Chunk;
    class Tile;
    /**
     * The TileLayer component is designed to operate only at the world origin (0,0).
     * The position of the Transform2DComponent does not affect the coordinate transformation.
     * TileLayer组件被设计为只能在世界原点（0,0）工作。
     * Transform2DComponent组件的位置不会影响坐标转换。
     */
    class TileLayerComponent final : public GameComponent
    {
        WorldContext* worldContext_;
        TileLayerType tileLayerType_;
        TileVector2D focusPosition_ = TileVector2D{};


        [[nodiscard]] std::shared_ptr<Tile>
        GetTileShared(TileLayerType layerType, const TileVector2D& tilePos) const;

        [[nodiscard]] static std::unique_ptr<std::vector<TileSnapshot>> GetTopVisibleTileSnapshots(
            const Chunk* chunk, uint8_t layerFilter,
            const TileVector2D& tilePos);

        [[nodiscard]] TileStateMessage* GetTileStatePtr(
            TileLayerType layerType,
            const TileVector2D& tilePos) const;

    public:
        [[nodiscard]] static uint64_t GenerateTileFingerprint(const TileVector2D& tileTopLeftPosition,
                                                              TileLayerType tileLayerType);

        [[nodiscard]] const Tile* GetTile(TileLayerType layerType, const TileVector2D& tilePos) const;

        /**
         *Tile To World
         * 瓦片坐标转世界坐标
         * @param tilePos TileVector2D 瓦片坐标
         * @return WorldVector2D(The upper left corner of the tile) 世界坐标（瓦片左上角）
         */
        [[nodiscard]] static WorldVector2D TileToWorld(const TileVector2D& tilePos);

        /**
        * Tile To World
         * 瓦片坐标转世界坐标
         * @param tilePos TileVector2D 瓦片坐标
         * @return WorldVector2D(The center of the tile) 世界坐标（瓦片中心）
         */
        [[nodiscard]] static WorldVector2D TileToWorldCenter(const TileVector2D& tilePos);

        /**
         * WorldToTile
         * 世界坐标转瓦片坐标
         * @param worldPos worldPos 世界坐标
         * @return
         */
        [[nodiscard]] static TileVector2D WorldToTile(const WorldVector2D& worldPos);

        /**
         * Get Top Visible Tiles In Viewport
         * 获取视口矩形内的所有顶层可见瓦片数据
         * @param layerFilter The layer to be queried 要查询的图层
         * @param worldViewport
         * @return
         */
        [[nodiscard]] std::vector<std::pair<TileVector2D, std::unique_ptr<std::vector<TileSnapshot>>>>
        GetTopVisibleTileSnapshotsInViewport(
            uint8_t layerFilter,
            const SDL_FRect& worldViewport) const;


        [[nodiscard]] const Tile* GetSelfLayerTile(const TileVector2D& tilePos) const;

        [[nodiscard]] std::shared_ptr<Tile> GetSelfLayerTileShared(const TileVector2D& tilePos) const;

        [[nodiscard]] bool CommitTileState(
            TileLayerType layerType, const TileVector2D& tilePos, bool fallback) const;


        [[nodiscard]] const TileStateMessage* GetSelfLayerTileState(
            const TileVector2D& tilePos) const;

        [[nodiscard]] TileStateMessage* GetSelfLayerTileStateMutable(
            const TileVector2D& tilePos) const;

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
        [[nodiscard]] const TileVector2D& GetFocusPosition() const;


        explicit TileLayerComponent(WorldContext* worldContext,
                                    const TileLayerType tileLayerType) : worldContext_(worldContext), tileLayerType_(
                                                                             tileLayerType)
        {
        }

        [[nodiscard]] GameComponentTypeMessage GetComponentType() override;
    };
}
