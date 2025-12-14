//
// Created by Cold-Mint on 2025/12/3.
//

#ifndef GLIMMERWORKS_TILEMANAGER_H
#define GLIMMERWORKS_TILEMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "../Resource.h"

namespace glimmer {
    class TileManager {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, TileResource> > tileMap_{};

        /**
         * Air
         * 空气
         */
        std::unique_ptr<TileResource> air = nullptr;
        /**
         * Water
         * 水
         */
        std::unique_ptr<TileResource> water = nullptr;

    public:
        /**
         * Initialize the built-in tiles
         * 初始化内置瓦片
         */
        void InitBuiltinTiles();

        /**
         * GetAir
         * 获取空气瓦片
         * @return
         */
        [[nodiscard]] TileResource *GetAir() const;

        [[nodiscard]] TileResource *GetWater() const;

        void RegisterResource(TileResource &tileResource);

        [[nodiscard]] TileResource *Find(const std::string &packId, const std::string &key);

        std::string ListTiles() const;
    };
}


#endif //GLIMMERWORKS_TILEMANAGER_H
