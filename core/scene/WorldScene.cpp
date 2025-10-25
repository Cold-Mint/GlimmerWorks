//
// Created by Cold-Mint on 2025/10/25.
//

#include "WorldScene.h"

bool Glimmer::WorldScene::HandleEvent(const SDL_Event &event) {
    return false;
}

void Glimmer::WorldScene::Update(float delta) {
}

void Glimmer::WorldScene::Render(SDL_Renderer *renderer) {
    if (!worldContext) return;

    const int TILE_SIZE = 32;
    int w = 0, h = 0;
    SDL_GetWindowSize(appContext->window, &w, &h);

    for (const auto &chunkPtr: worldContext->chunks) {
        const auto &chunk = *chunkPtr;

        for (int localX = 0; localX < CHUNK_SIZE; ++localX) {
            for (int localY = 0; localY < CHUNK_SIZE; ++localY) {
                const std::string &tileID = chunk.tileIDs[localX][localY];

                SDL_Color color;
                if (tileID == "dirt") {
                    color = {139, 69, 19, 255};
                } else if (tileID == TILE_ID_AIR) {
                    color = {135, 206, 235, 255};
                } else {
                    color = {255, 0, 255, 255};
                }

                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

                SDL_FRect dstRect;
                dstRect.x = static_cast<float>(chunk.origin.x + localX) * TILE_SIZE;
                dstRect.y = static_cast<float>(h - (chunk.origin.y + localY + 1) * TILE_SIZE); // 翻转 Y
                dstRect.w = static_cast<float>(TILE_SIZE);
                dstRect.h = static_cast<float>(TILE_SIZE);

                SDL_RenderFillRect(renderer, &dstRect);
            }
        }
    }
}
