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
#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "core/mod/Resource.h"
#include "core/world/generator/TileLayerType.h"

namespace glimmer
{
    enum class TilePhysicsType : uint8_t;

    struct CoreTileResourceParams
    {
        std::string resourceId;
        TilePhysicsType physicsType;
        TileLayerType layerType;
        float unitHardness;
        std::string nameKey;
        std::string textureKey;
        std::string lightSourceKey;
        std::string sideLightMaskKey;
        std::string backLightMaskKey;
        bool isOverwritable;
        bool canDropLoot;
        std::optional<std::string> descriptionKey;
    };

    class TileResourceManager
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<TileResource>,
                                                           TransparentStringHash, std::equal_to<>>,
                           TransparentStringHash, std::equal_to<>> tileMap_{};

        std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::unique_ptr<TileResource>,
                                                                TransparentStringHash, std::equal_to<>>,
                           TransparentStringHash, std::equal_to<>>
        accessDeniedTileMap_{};

        TileResource* air_ = nullptr;
        TileResource* airWall_ = nullptr;

        TileResource* AddCoreResource(const CoreTileResourceParams& params);

        static ResourceRef CreateCoreRef(const std::string& key, ResourceTypeMessage type);

    public:
        TileResourceManager();

        /**
         * Initialize the built-in tiles
         * 初始化内置瓦片
         */
        void InitBuiltinTiles();

        [[nodiscard]] TileResource* AddErrorPlaceHolder(
            std::string_view packId, std::string_view resourceId, TileLayerType tileLayer);

        [[nodiscard]] TileResource* GenerateAccessDeniedPlaceHolder(
            std::string_view packId, std::string_view resourceId, TileLayerType tileLayer);

        TileResource* AddResource(std::unique_ptr<TileResource> tileResource);

        /**
         * Obtain the air resources at the corresponding location.
         * 获取对应位置的空气资源。
         * @param tileLayerType
         * @return
         */
        [[nodiscard]] TileResource* GetAirResource(TileLayerType tileLayerType) const;

        [[nodiscard]] static uint64_t GetAirResourceRefFingerprint(TileLayerType tileLayerType);

        [[nodiscard]] static ResourceRef GetAirResourceRef(TileLayerType tileLayerType);

        /**
         * Search for the tile. If not found, return nullptr.
         * 查找瓦片，找不到时返回nullptr
         * @param packId
         * @param key
         * @return
         */
        [[nodiscard]] TileResource* FindTileRaw(std::string_view packId, std::string_view key);


        /**
         * Search for tiles. If no tiles are found, generate error tiles and placeholder tiles instead.
         * 查找瓦片，找不到时生成错误瓦片占位瓦片。
         * @param packId
         * @param key
         * @param tileLayer
         * @return
         */
        [[nodiscard]] TileResource* FindTileFallback(std::string_view packId, std::string_view key,
                                                     TileLayerType tileLayer);

        [[nodiscard]] std::vector<std::string> GetTileIDList() const;

        std::string ListTiles() const;
    };
}
