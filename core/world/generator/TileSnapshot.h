//
// Created by coldmint on 2026/5/24.
//

#pragma once


class TileStateMessage;

namespace glimmer {
    class Tile;

    class TileSnapshot {
        const Tile *tile_ = nullptr;
        const TileStateMessage *tileState_ = nullptr;
    public:
        void SetTile(const Tile *tile);

        [[nodiscard]] const Tile *GetTile() const;

        void SetTileState(const TileStateMessage *tileState);

        [[nodiscard]] const TileStateMessage *GetTileState() const;
    };
}
