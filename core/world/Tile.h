//
// Created by Cold-Mint on 2025/11/2.
//

#ifndef GLIMMERWORKS_TILE_H
#define GLIMMERWORKS_TILE_H
#include <memory>
#include <SDL3/SDL.h>

#include "TileLayerType.h"
#include "TilePhysicsType.h"
#include "../mod/Resource.h"

namespace glimmer {
    class AppContext;

    struct Tile {
        std::string id;
        std::string name;
        std::string description;
        std::shared_ptr<SDL_Texture> texture;
        float hardness = 1.0F;
        bool breakable = true;
        TilePhysicsType physicsType = TilePhysicsType::None;
        TileLayerType layerType = TileLayerType::Main;

        /**
         * From Resource Ref
         * 从资源引用创建瓦片
         * @param appContext appContext 应用上下文
         * @param tileResource tileResource 瓦片资源
         * @return
         */
        static std::unique_ptr<Tile> FromResourceRef(AppContext *appContext, const TileResource *tileResource);
    };
}


#endif //GLIMMERWORKS_TILE_H
