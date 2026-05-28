//
// Created by Cold-Mint on 2026/5/24.
//

#include "TileSnapshot.h"


void glimmer::TileSnapshot::SetTile(const Tile *tile) {
    tile_ = tile;
}

const glimmer::Tile *glimmer::TileSnapshot::GetTile() const {
    return tile_;
}

void glimmer::TileSnapshot::SetTileState(const TileStateMessage *tileState) {
    tileState_ = tileState;
}

const TileStateMessage *glimmer::TileSnapshot::GetTileState() const {
    return tileState_;
}
