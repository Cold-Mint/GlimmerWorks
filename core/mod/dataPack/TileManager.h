//
// Created by Cold-Mint on 2025/12/3.
//

#ifndef GLIMMERWORKS_TILEMANAGER_H
#define GLIMMERWORKS_TILEMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "../Resource.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer {
    enum class TilePhysicsType : uint8_t;

    class TileManager {
    protected:
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<TileResource> > > tileMap_{};

        //Save all the tiles that were created but were denied access.
        //保存所有被拒绝访问创建的瓦片。
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<TileResource> > >
        accessDeniedTileMap_{};

        TileResource *air_ = nullptr;
        TileResource *airWall_ = nullptr;

        TileResource *AddCoreResource(const std::string &resourceId, TilePhysicsType physicsType,
                                      TileLayerType layerType, float hardness, const std::string &nameKey,
                                      const std::string &textureKey, const std::string &lightSourceKey,
                                      const std::string &lightMaskKey,
                                      std::optional<std::string> descriptionKey);

    public:
        /**
         * Initialize the built-in tiles
         * 初始化内置瓦片
         */
        void InitBuiltinTiles();

        [[nodiscard]] TileResource *AddErrorPlaceHolder(
            const std::string &packId, const std::string &resourceId);

        [[nodiscard]] TileResource *GenerateAccessDeniedPlaceHolder(
            const std::string &packId, const std::string &resourceId);

        [[nodiscard]] TileResource *AddResource(std::unique_ptr<TileResource> tileResource);

        /**
         * Obtain the air resources at the corresponding location.
         * 获取对应位置的空气资源。
         * @param tileLayerType
         * @return
         */
        [[nodiscard]] TileResource *GetAirResource(TileLayerType tileLayerType) const;

        [[nodiscard]] static ResourceRef GetAirResourceRef(TileLayerType tileLayerType);

        [[nodiscard]] TileResource *Find(const std::string &packId, const std::string &key);

        [[nodiscard]] std::vector<std::string> GetTileIDList();

        std::string ListTiles() const;
    };
}


#endif //GLIMMERWORKS_TILEMANAGER_H
